/*
 *   Copyright (C) 2017,2018 by Andy Uribe CA6JAU
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

#if !defined(CONFIG_H)
#define  CONFIG_H

// Select one board (STM32F103 based boards)
// 1) ZUMspot RPi or ZUMspot USB:
#define ZUMSPOT_ADF7021
// 2) Libre Kit board or any homebrew hotspot with modified RF7021SE and Blue Pill STM32F103:
// #define LIBRE_KIT_ADF7021
// 3) MMDVM_HS_Hat revisions 1.1, 1.2 and 1.4 (DB9MAT & DF2ET)
// #define MMDVM_HS_HAT_REV12
// 4) Nano hotSPOT (BI7JTA)
// #define NANO_HOTSPOT
// 5) Nano DV revisions 1.0 (BG4TGO & BG5HHP)
// #define NANO_DV_REV10

// Enable ADF7021 support:
#define ENABLE_ADF7021

// TCXO of the ADF7021
// For 14.7456 MHz:
#define ADF7021_14_7456
// For 12.2880 MHz:
// #define ADF7021_12_2880

// POCSAG configuration
#define FREQ_RX 434600000
#define FREQ_TX 434600000
#define CAPCODE 995814

// Send RSSI value:
// #define SEND_RSSI_DATA

// Constant Service LED once repeater is running 
// #define CONSTANT_SRV_LED

#endif
