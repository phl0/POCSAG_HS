/*
 *   Copyright (C) 2009-2017 by Jonathan Naylor G4KLX
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
#include "POCSAGRX.h"
#include "Utils.h"

const uint8_t FSC_BIT_START_ERRS = 2U;
const uint8_t MAX_FSC_BIT_RUN_ERRS = 3U;

const unsigned int MAX_FSC_FRAMES = 4U;

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])


CPOCSAGRX::CPOCSAGRX() :
m_prev(false),
m_state(POCSAGRXS_NONE),
m_bitBuffer(0x00U),
m_buffer(),
m_bufferPtr(0U),
m_lostCount(0U)
{
}

void CPOCSAGRX::reset()
{
  m_prev      = false;
  m_state     = POCSAGRXS_NONE;
  m_bitBuffer = 0x00U;
  m_bufferPtr = 0U;
  m_lostCount = 0U;
}

void CPOCSAGRX::databit(bool bit)
{
  if (m_state == POCSAGRXS_NONE)
    processNone(bit);
  else
    processData(bit);
}

void CPOCSAGRX::processNone(bool bit)
{
  m_bitBuffer <<= 1;
  if (bit)
    m_bitBuffer |= 0x01U;

  // Fuzzy matching of the data sync bit sequence
  if (countBits32((m_bitBuffer & POCSAG_FSC_MASK) ^ POCSAG_FSC_WORD) <= FSC_BIT_START_ERRS) {
    for (uint8_t i = 0U; i < POCSAG_FSC_BYTES; i++)
      m_buffer[i] = POCSAG_FSC[i];

    m_lostCount = MAX_FSC_FRAMES;
    m_bufferPtr = POCSAG_FSC_BITS;
    m_state     = POCSAGRXS_DATA;

    io.setDecode(true);
  }
}

void CPOCSAGRX::processData(bool bit)
{
  m_bitBuffer <<= 1;
  if (bit)
    m_bitBuffer |= 0x01U;

  WRITE_BIT1(m_buffer, m_bufferPtr, bit);
  m_bufferPtr++;

  if (m_bufferPtr >= (POCSAG_FSC_BITS - 2U) && m_bufferPtr <= (POCSAG_FSC_BITS + 2U)) {
    // Fuzzy matching of the data sync bit sequence
    if (countBits32((m_bitBuffer & POCSAG_FSC_MASK) ^ POCSAG_FSC_WORD) <= MAX_FSC_BIT_RUN_ERRS) {
      m_lostCount = MAX_FSC_FRAMES;
      m_bufferPtr = POCSAG_FSC_BITS;
      io.setDecode(true);
    }
  }

  if (m_bufferPtr == POCSAG_FRAME_LENGTH_BITS) {
    m_lostCount--;
    io.setDecode(false);
    m_bufferPtr = 0U;

    if (m_lostCount == 0U)
      m_state = POCSAGRXS_NONE;
    else
      pocsagDec.addData(m_buffer);
  }

}
