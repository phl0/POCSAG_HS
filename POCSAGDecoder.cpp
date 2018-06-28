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

CPOCSAGDecoder::CPOCSAGDecoder() :
m_bufferRX(1000U),
m_state(POCSAG_IDLE)
{
  m_ric = RIC_NUMBER;
  m_address_cw = (m_ric / POCSAG_FRAME_ADDRESSES) << 13;
  m_frame_pos = m_ric % POCSAG_FRAME_ADDRESSES;
  m_addr_pos = (8U * m_frame_pos) + 4U;
}

void CPOCSAGDecoder::addData(uint8_t* data)
{
  if (m_state == POCSAG_IDLE) {
    if (checkAddress(data))
      m_state = POCSAG_START;
  }

  // Copy data only if the address matches with our RIC number
  if (m_state != POCSAG_IDLE) {
    for (uint8_t i = 0U; i < POCSAG_FRAME_LENGTH_BYTES; i++)
      m_bufferRX.put(data[i]);

    io.DEB_pin(HIGH); // debug test pin
  }
  else
    io.DEB_pin(LOW); // debug test pin
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

  uint16_t errors = 0U;
  uint8_t count = 0U;
  uint8_t func = 0U;

  while (count < POCSAG_FRAME_LENGTH_WORDS) {
    switch (m_state) {
      case POCSAG_START:
      case POCSAG_IDLE:
        // Check address again, extract functional bits
        count = (2U * m_frame_pos) + 1U;
        if (checkAddress(m_words[count], func, errors)) {
          count++;
          m_state = POCSAG_MSG;
        } else {
          // Not for us, discard batch
          count = POCSAG_FRAME_LENGTH_WORDS;
          m_state = POCSAG_IDLE;
        }
        break;
      case POCSAG_MSG:
        // Check and fix errors
        if (pocsagFEC.decode(m_words[count], errors)) {
          // See if the codeword is a message
          if (m_words[count] & POCSAG_MSG_MASK) {
            // Do something with the data received...
            count++;
          }
          else {
            count = POCSAG_FRAME_LENGTH_WORDS;
            m_state = POCSAG_IDLE;
          }
        } else {
          // Codeword too corrupt
          count++;
        }
        break;
      default:
        break;
    }
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

