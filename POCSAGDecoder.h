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

#if !defined(POCSAGDECODER_H)
#define  POCSAGDECODER_H

#include "POCSAGDefines.h"

enum POCSAG_RXS {
  POCSAG_IDLE,
  POCSAG_START,
  POCSAG_MSG
};

class CPOCSAGDecoder {
public:
  CPOCSAGDecoder();

  void addData(uint8_t* data);
  void setIdle();

  void process();

private:
  bool checkAddress(uint8_t* data);
  bool checkAddress(uint32_t addr_cw, uint8_t& func, uint16_t& errors);
  void toASCII(uint32_t cw);
  void toNumeric(uint32_t cw);
  void toAlpha(uint32_t cw);

  CSerialRB    m_bufferRX;
  uint8_t      m_char_out[1024];
  uint32_t     m_words[POCSAG_FRAME_LENGTH_WORDS];
  uint32_t     m_ric;
  uint8_t      m_frame_pos;
  uint16_t     m_addr_pos;
  uint32_t     m_address_cw;
  POCSAG_RXS   m_state;
  uint8_t      m_func;
  uint16_t     m_errors;
  int16_t      m_rssi;
  uint8_t      m_cw;
  uint16_t     m_len;

};

#endif
