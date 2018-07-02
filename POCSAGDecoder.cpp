/*
 *   Copyright (C) 2018 by Andy Uribe CA6JAU
 *   Copyright (C) 2009-2017 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Config.h"
#include "Globals.h"
#include "Utils.h"

static const uint8_t BCD_TABLE[] = { '0', '8', '4', ' ', '2', 'S', '6', ')',
                                     '1', '9', '5', '-', '3', 'U', '7', '(' };

static const uint8_t INVBITS_TABLE[] = { 0x00, 0x40, 0x20, 0x60,
                                         0x10, 0x50, 0x30, 0x70,
                                         0x08, 0x48, 0x28, 0x68,
                                         0x18, 0x58, 0x38, 0x78,
                                         0x04, 0x44, 0x24, 0x64,
                                         0x14, 0x54, 0x34, 0x74,
                                         0x0c, 0x4c, 0x2c, 0x6c,
                                         0x1c, 0x5c, 0x3c, 0x7c,
                                         0x02, 0x42, 0x22, 0x62,
                                         0x12, 0x52, 0x32, 0x72,
                                         0x0a, 0x4a, 0x2a, 0x6a,
                                         0x1a, 0x5a, 0x3a, 0x7a,
                                         0x06, 0x46, 0x26, 0x66,
                                         0x16, 0x56, 0x36, 0x76,
                                         0x0e, 0x4e, 0x2e, 0x6e,
                                         0x1e, 0x5e, 0x3e, 0x7e,
                                         0x01, 0x41, 0x21, 0x61,
                                         0x11, 0x51, 0x31, 0x71,
                                         0x09, 0x49, 0x29, 0x69,
                                         0x19, 0x59, 0x39, 0x79,
                                         0x05, 0x45, 0x25, 0x65,
                                         0x15, 0x55, 0x35, 0x75,
                                         0x0d, 0x4d, 0x2d, 0x6d,
                                         0x1d, 0x5d, 0x3d, 0x7d,
                                         0x03, 0x43, 0x23, 0x63,
                                         0x13, 0x53, 0x33, 0x73,
                                         0x0b, 0x4b, 0x2b, 0x6b,
                                         0x1b, 0x5b, 0x3b, 0x7b,
                                         0x07, 0x47, 0x27, 0x67,
                                         0x17, 0x57, 0x37, 0x77,
                                         0x0f, 0x4f, 0x2f, 0x6f,
                                         0x1f, 0x5f, 0x3f, 0x7f };

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])

CPOCSAGDecoder::CPOCSAGDecoder() :
m_bufferRX(1000U),
m_state(POCSAG_IDLE),
m_func(0U),
m_errors(0U),
m_rssi(0U),
m_cw(0U),
m_len(0U),
m_len_bit(0U)
{
  m_ric = RIC_NUMBER;
  m_address_cw = (m_ric / POCSAG_FRAME_ADDRESSES) << 13;
  m_frame_pos = m_ric % POCSAG_FRAME_ADDRESSES;
  m_addr_pos = (8U * m_frame_pos) + 4U;
}

void CPOCSAGDecoder::addData(uint8_t* data)
{
  // Check if address matches with RIC
  if (m_state == POCSAG_IDLE) {
    if (checkAddress(data))
      m_state = POCSAG_START;
  }

  // Start copying data when the address matches with our RIC number
  if (m_state != POCSAG_IDLE) {
    for (uint8_t i = 0U; i < POCSAG_FRAME_LENGTH_BYTES; i++)
      m_bufferRX.put(data[i]);

    io.DEB_pin(HIGH); // debug pin
  }
}

void CPOCSAGDecoder::setIdle()
{
  m_state = POCSAG_IDLE;
  io.DEB_pin(LOW); // debug pin
}

void CPOCSAGDecoder::process()
{
  // Check if we have data for us
  if (m_bufferRX.getData() < POCSAG_FRAME_LENGTH_BYTES)
    return;

  // Clear work buffer
  ::memset(m_words, 0U, POCSAG_FRAME_LENGTH_BYTES);

  // Transform bytes to 32 bits words
  for (uint8_t i = 0U; i < POCSAG_FRAME_LENGTH_BYTES; i++) {
    m_words[i>>2] |= m_bufferRX.get() << 8 * (3 - (i % 4));
  }

  uint8_t count = 1U;

  while (count < POCSAG_FRAME_LENGTH_WORDS) {
    switch (m_state) {
      case POCSAG_START:
      case POCSAG_IDLE:
        m_errors = 0U;

        // Check address again, extract functional bits
        count = (2U * m_frame_pos) + 1U;
        if (checkAddress(m_words[count], m_func, m_errors)) {
          count++;
          m_cw = 1U;
          m_len = 0U;
          m_len_bit = 0U;
          m_state = POCSAG_MSG;
          m_rssi = io.readRSSI();
        } else {
          // Not for us, discard batch
          count = POCSAG_FRAME_LENGTH_WORDS;
          m_state = POCSAG_START;
        }
        break;
      case POCSAG_MSG:
        // Check and fix errors
        if (pocsagFEC.decode(m_words[count], m_errors)) {
          // See if the codeword is a message
          if (m_words[count] & POCSAG_MSG_MASK) {
            // Convert data received to ASCII
            toASCII(m_words[count]);
            count++;
            m_cw++;
          } else {
            // Check for contiguous new msg, check address, etc
            uint16_t addr_pos = (2U * m_frame_pos) + 1U;
            if (count < addr_pos)
              count = addr_pos;
            if (checkAddress(m_words[count], m_func, m_errors)) {
              // Send msg to display
              msgReady();
              count++;
              m_cw = 1U;
              m_len = 0U;
              m_len_bit = 0U;
              m_state = POCSAG_MSG;
              m_rssi = io.readRSSI();
            } else {
              // End of the message
              // Send msg to display
              msgReady();
              count = POCSAG_FRAME_LENGTH_WORDS;
              m_state = POCSAG_START;
            }
            m_errors = 0U;
          }
        } else {
          // Codeword too corrupt
          // Try to convert to ASCII anyway...
          toASCII(m_words[count]);
          count++;
          m_cw++;
        }
        break;
      default:
        break;
    }
  }
}

void CPOCSAGDecoder::msgReady()
{
  // Invert bits if the msg is alphanumeric
  if (m_func == 3U) {
    for (uint8_t k = 0U; k < m_len; k++)
      m_char_out[k] = INVBITS_TABLE[m_char_out[k]];
  }

  display.showMsg(m_char_out, m_len, m_cw, m_errors, m_rssi);
}

void CPOCSAGDecoder::toASCII(uint32_t cw)
{
  //serial.writeInt2Hex((uint8_t*)"RX: ", cw);
  if (m_func == 0U)
    toNumeric(cw);
  else if (m_func == 3U)
    toAlpha(cw);
}

void CPOCSAGDecoder::toNumeric(uint32_t cw)
{
  for (uint8_t k = 0; k < 5U; k++) {
    m_char_out[m_len] = BCD_TABLE[(cw >> (27 - (4 * k))) & 0x0F];
    m_len++;
  }
}

void CPOCSAGDecoder::toAlpha(uint32_t cw)
{
  for (uint8_t k = 0U; k < 20U; k++) {
    if ((m_len_bit % 8U) == 0U) {
      WRITE_BIT1(m_char_out, m_len_bit, 0);
      m_len_bit++;
      m_len++;
    }
    bool bit = (cw >> (30 - k)) & 0x01;
    WRITE_BIT1(m_char_out, m_len_bit, bit);
    m_len_bit++;
  }
}

bool CPOCSAGDecoder::checkAddress(uint8_t* data)
{
  uint16_t errors = 0U;

  // Extract address codeword at the expected position
  uint32_t addr_cw = data[m_addr_pos + 0U] << 24;
  addr_cw |= data[m_addr_pos + 1U] << 16;
  addr_cw |= data[m_addr_pos + 2U] << 8;
  addr_cw |= data[m_addr_pos + 3U];

  // Check and correct for errors
  if (pocsagFEC.decode(addr_cw, errors)) {
    if (addr_cw & POCSAG_MSG_MASK) {
      return false; // received a message codeword, not address
    } else if (addr_cw == POCSAG_IDLE_WORD) {
      return false; // received a IDLE codeword
    } else if ((addr_cw & POCSAG_ADDR_MASK) == m_address_cw) {
      return true;  // address for us !
    } else
      return false;
  } else
    return false; // uncorrectable error
}

bool CPOCSAGDecoder::checkAddress(uint32_t addr_cw, uint8_t& func, uint16_t& errors)
{
  // Check and correct for errors
  if (pocsagFEC.decode(addr_cw, errors)) {
    if (addr_cw & POCSAG_MSG_MASK) {
      return false; // received a message codeword, not address
    } else if (addr_cw == POCSAG_IDLE_WORD) {
      return false; // received a IDLE codeword
    } else if ((addr_cw & POCSAG_ADDR_MASK) == m_address_cw) {
      func = (addr_cw & POCSAG_FUNC_MASK) >> 11;
      return true;  // address for us !
    } else
      return false;
  } else
    return false; // uncorrectable error
}

