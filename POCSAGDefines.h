/*
 *   Copyright (C) 2009-2018 by Jonathan Naylor G4KLX
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

#if !defined(POCSAGDEFINES_H)
#define  POCSAGDEFINES_H

const uint16_t POCSAG_PREAMBLE_LENGTH_BYTES = 576U / 8U;

const uint16_t POCSAG_FRAME_LENGTH_BYTES = 17U * sizeof(uint32_t);
const uint16_t POCSAG_FRAME_LENGTH_BITS = POCSAG_FRAME_LENGTH_BYTES * 8U;

const unsigned int POCSAG_FRAME_ADDRESSES = 8U;

const uint16_t POCSAG_FSC_BYTES = 4U;
const uint16_t POCSAG_FSC_BITS = POCSAG_FSC_BYTES * 8U;
const uint32_t POCSAG_FSC_WORD = 0x7CD215D8U;
const uint8_t  POCSAG_FSC[] = {0x7CU, 0xD2U, 0x15U, 0xD8U};
const uint32_t POCSAG_FSC_MASK = 0xFFFFFFFFU;

const uint32_t POCSAG_IDLE_WORD = 0x7A89C197U;

const uint8_t  POCSAG_SYNC = 0xAAU;

#endif
