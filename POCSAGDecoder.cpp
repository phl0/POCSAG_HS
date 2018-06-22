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

const uint8_t MAX_BIT_ERRS = 2U;

CPOCSAGDecoder::CPOCSAGDecoder() :
m_bufferRX(1000U)
{
  m_ric = RIC_NUMBER;
  m_frame_pos = m_ric % POCSAG_FRAME_ADDRESSES;

  genAddress();
}

void CPOCSAGDecoder::addData(uint8_t* data)
{
  for (uint8_t i = 0U; i < POCSAG_FRAME_LENGTH_BYTES; i++)
    m_bufferRX.put(data[i]);
}

void CPOCSAGDecoder::process()
{
  if (m_bufferRX.getData() < POCSAG_FRAME_LENGTH_BYTES)
    return;

  ::memset(m_words, 0U, POCSAG_FRAME_LENGTH_BYTES);

  for (uint8_t i = 0U; i < POCSAG_FRAME_LENGTH_BYTES; i++) {
    m_words[i>>2] |= m_bufferRX.get() << 8 * (3 - (i % 4));
  }

  if (checkAddress(m_words))
    io.DEB_pin(HIGH); // debug test pin
}

bool CPOCSAGDecoder::checkAddress(uint32_t* data)
{
  if (countBits32(data[2U*m_frame_pos + 1U] ^ m_address) <= MAX_BIT_ERRS)
    return true;
  else
    return false;
}

bool CPOCSAGDecoder::isIdle(uint32_t &data)
{
  if (countBits32(data ^ POCSAG_IDLE_WORD) <= MAX_BIT_ERRS)
    return true;
  else
    return false;
}

// Functions from MMDVMHost (POCSAGControl.cpp), Jonathan Naylor, G4KLX:

void CPOCSAGDecoder::genAddress()
{
  m_address = 0x00001800U; // Alphanumeric for now, just testing...
  m_address |= (m_ric / POCSAG_FRAME_ADDRESSES) << 13;
  addBCHAndParity(m_address);
}

void CPOCSAGDecoder::addBCHAndParity(uint32_t& word)
{
  uint32_t temp = word;

  for (unsigned int i = 0U; i < 21U; i++, temp <<= 1) {
    if (temp & 0x80000000U)
      temp ^= 0xED200000U;
  }

  word |= (temp >> 21);

  temp = word;

  unsigned int parity = 0U;
  for (unsigned int i = 0U; i < 32U; i++, temp <<= 1) {
    if (temp & 0x80000000U)
      parity++;
  }

  if ((parity % 2U) == 1U)
    word |= 0x00000001U;
}
