/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
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

#if !defined(GLOBALS_H)
#define  GLOBALS_H

#if defined(STM32F10X_MD)
#include <stm32f10x.h>
#include "string.h"
#elif defined(STM32F4XX)
#include "stm32f4xx.h"
#include "string.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#include "string.h"
#else
#include <Arduino.h>
#endif

// Bidirectional Data pin (Enable Standard TX/RX Data Interface of ADF7021):
#define BIDIR_DATA_PIN

#include "IO.h"

#include "SerialRB.h"
#include "POCSAGRX.h"
#include "POCSAGTX.h"

const uint16_t TX_RINGBUFFER_SIZE = 1024U;
const uint16_t RX_RINGBUFFER_SIZE = 1024U;

extern bool m_tx;
extern bool m_dcd;

extern CIO io;

extern CPOCSAGRX pocsagRX;
extern CPOCSAGTX pocsagTX;

#endif

