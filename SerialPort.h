/*
 *   Copyright (C) 2015,2016,2018 by Jonathan Naylor G4KLX
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

#if !defined(SERIALPORT_H)
#define  SERIALPORT_H

#include "Globals.h"

class CSerialPort {
public:
  CSerialPort();

  void start();

  void process();
  
  // Hardware versions
  void    begin(uint8_t n, int speed);
  int     available(uint8_t n);
  uint8_t read(uint8_t n);
  void    write(uint8_t n, const uint8_t* data, uint16_t length, bool flush = false);
  
#if defined(ENABLE_DEBUG)
  void    writeInt2Hex(const uint8_t* text, uint32_t n1);
#endif

private:  

};

#if defined(ENABLE_DEBUG)
uint16_t uintToHexStr(uint32_t num, uint8_t* buff);
#endif

#endif

