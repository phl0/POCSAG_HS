/*
 *   Copyright (C) 2016 by Jim McLaughlin KI6ZUM
 *   Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
 *   Copyright (C) 2017 by Danilo DB4PLE 
 *
 *   Some of the code is based on work of Guus Van Dooren PE1PLM:
 *   https://github.com/ki6zum/gmsk-dstar/blob/master/firmware/dvmega/dvmega.ino
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
 
#if !defined(ADF7021_H)
#define  ADF7021_H

#include "Config.h"

#if defined(ENABLE_ADF7021)

/*
- Most of the registers values are obteined from ADI eval software:
http://www.analog.com/en/products/rf-microwave/integrated-transceivers-transmitters-receivers/low-power-rf-transceivers/adf7021.html
- or ADF7021 datasheet formulas:
www.analog.com/media/en/technical-documentation/data-sheets/ADF7021.pdf
*/

/***** Test modes ****/

// Enable SWD pin to access the demodulator output signal:
// (See application note AN-852 and ADF7021 datasheet, page 60)
// #define TEST_DAC

// Transmit the carrier frequency:
// #define TEST_TX

/****** Support for 14.7456 MHz TCXO (modified RF7021SE boards) ******/
#if defined(ADF7021_14_7456)

// R = 4
// DEMOD_CLK = 7.3728 MHz (POCSAG)
#define ADF7021_PFD              3686400.0

// PLL (REG 01)
#define ADF7021_REG1_VHF1        0x021F5041
#define ADF7021_REG1_VHF2        0x021F5041
#define ADF7021_REG1_UHF1        0x00575041
#define ADF7021_REG1_UHF2        0x00535041

// Deviation of modulator (REG 02)
#define ADF7021_DEV_POCSAG       160U

// TX/RX CLOCK register (REG 03)
#define ADF7021_REG3_POCSAG      0x2A4F0093

// Discriminator bandwith, demodulator (REG 04)
#define ADF7021_DISC_BW_POCSAG   406U // K=22

// Post demodulator bandwith (REG 04)
#define ADF7021_POST_BW_POCSAG   1U

// IF filter (REG 05)
#define ADF7021_REG5             0x000024F5

// IF CAL (fine cal, defaults) (REG 06)
#define ADF7021_REG6             0x05070E16

// AFC configuration (REG 10)
#define ADF7021_REG10_POCSAG     0x1496473A

/****** Support for 12.2880 MHz TCXO ******/
#elif defined(ADF7021_12_2880)

// R = 2
// DEMOD_CLK = 6.1440 MHz (POCSAG)
#define ADF7021_PFD              6144000.0

// PLL (REG 01)
#define ADF7021_REG1_VHF1        0x021F5021
#define ADF7021_REG1_VHF2        0x021F5021
#define ADF7021_REG1_UHF1        0x00575021
#define ADF7021_REG1_UHF2        0x00535021

// Deviation of modulator (REG 02)
#define ADF7021_DEV_POCSAG       96U

// TX/RX CLOCK register (REG 03)
#define ADF7021_REG3_POCSAG      0x29EE8093

// Discriminator bandwith, demodulator (REG 04)
#define ADF7021_DISC_BW_POCSAG   338U // K=22

// Post demodulator bandwith (REG 04)
#define ADF7021_POST_BW_POCSAG   1U

// IF filter (REG 05)
#define ADF7021_REG5             0x00001ED5

// IF CAL (fine cal, defaults) (REG 06)
#define ADF7021_REG6             0x0505EBB6

// AFC (REG 10)
#define ADF7021_REG10_POCSAG     0x1496557A

#endif

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

void Send_AD7021_control(bool doSle = true);

#endif

#endif
