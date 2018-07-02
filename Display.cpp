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
#include <math.h>

CDisplay::CDisplay()
{
}

void CDisplay::init()
{
  // Call internal function to configure display
  initInt();
}

void CDisplay::showMsg(uint8_t* data, uint16_t length, uint8_t n_cw, uint16_t errors, int16_t rssi)
{
#if defined(ENABLE_DEBUG)
  // Copy msgs to the host serial port for debugging
  serial.write(1U, data, length, true);
  serial.writeNum((uint8_t*)"Total codewords:", n_cw);
  serial.writeNum((uint8_t*)"Total bit errors:", errors);
  serial.writeNum((uint8_t*)"BER x10 (\%):", (int32_t)round((31.25F * errors) / n_cw));
  serial.writeNum((uint8_t*)"RSSI:", rssi);
#endif

  // Call internal function to show msgs on the display
  showMsgInt(data, length, n_cw, errors, rssi);
}
