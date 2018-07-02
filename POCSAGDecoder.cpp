/*
 *   Copyright (C) 2018 by Andy Uribe CA6JAU
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

const uint8_t BCD_TABLE[] = {'0', '8', '4', ' ', '2', 'S', '6', ')', '1', '9', '5', '-', '3', 'U', '7', '('};

CPOCSAGDecoder::CPOCSAGDecoder() :
m_bufferRX(1000U),
m_state(POCSAG_IDLE),
m_func(0U),
m_errors(0U),
m_rssi(0U),
m_cw(0U),
m_len(0U)
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
              display.showMsg(m_char_out, m_len, m_cw, m_errors, m_rssi);
              count++;
              m_cw = 1U;
              m_len = 0U;
              m_state = POCSAG_MSG;
              m_rssi = io.readRSSI();
            } else {
              // End of the message
              // Send msg to display
              display.showMsg(m_char_out, m_len, m_cw, m_errors, m_rssi);
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
  for (uint8_t k=0; k<5U; k++) {
    m_char_out[m_len] = BCD_TABLE[(cw >> (27 - (4*k))) & 0x0F];
    m_len++;
  }
}

void CPOCSAGDecoder::toAlpha(uint32_t cw)
{

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

