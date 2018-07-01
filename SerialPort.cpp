/*
 *   Copyright (C) 2013,2015,2016,2018 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
 *   Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
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
#include "SerialPort.h"

CSerialPort::CSerialPort()
{
}

void CSerialPort::start()
{
  // Init host serial port
  begin(1U, 115200U);

  // Init serial port for Nextion display
  begin(3U, 9600U);
}

void CSerialPort::process()
{
  // Put code here for serial port RX
}

#if defined(ENABLE_DEBUG)
void CSerialPort::writeInt2Hex(const uint8_t* text, uint32_t n1)
{   
  uint8_t reply[64U];

  uint8_t count = 0U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' '; 
  reply[count++] = '0'; 
  reply[count++] = 'x'; 

  uintToHexStr(n1, &reply[count]);

  count += 8U;
  reply[count++] = '\r';
  reply[count++] = '\n';

  write(1U, reply, count, true);
}

// For the next function, please see here: http://blog.refu.co/?p=804
static uint8_t hex_data[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9' ,'A', 'B', 'C', 'D', 'E', 'F' };
//The function that performs the conversion. Accepts a buffer with "enough space" TM 
//and populates it with a string of hexadecimal digits.Returns the length in digits
uint16_t uintToHexStr(uint32_t num, uint8_t* buff)
{
  uint16_t len = 0, k = 0;
  do //for every 4 bits
  {
    //get the equivalent hex digit
    buff[len] = hex_data[num & 0xF];
    len++;
    num >>= 4;
  } while (len < 8);

  //since we get the digits in the wrong order reverse the digits in the buffer
  for(; k<len/2; k++)
  {//xor swapping
     buff[k] ^= buff[len - k - 1];
     buff[len-k-1] ^= buff[k];
     buff[k] ^= buff[len - k - 1];
  }

  return len;
}
#endif

