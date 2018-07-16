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
#include "Nextion.h"

#if defined(ENABLE_NEXTION)

void sendNextionCommand(const char* command)
{
  serial.write(3U, (uint8_t*)command, (unsigned int)::strlen(command));
  serial.write(3U, (uint8_t*)"\xFF\xFF\xFF", 3U, true);
}

void CDisplay::initInt()
{
  // Things to do...
  sendNextionCommand("bkcmd=0");
  sendNextionCommand("page IDLE");
  sendNextionCommand("t0.txt=\"DF2ET / 2024112\"");
}

void CDisplay::setDcdInt(bool dcd)
{
  if (dcd) {
    sendNextionCommand("p1.pic=2");
  } else {
    sendNextionCommand("p1.pic=1");
  }
}

void CDisplay::showMsgInt(uint8_t* data, uint16_t length, uint8_t n_cw, uint16_t errors, int16_t rssi)
{
  // Things to do...
}

#endif
