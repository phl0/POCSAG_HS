/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
 *   Copyright (C) 2017 by Danilo DB4PLE 
 
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
#include "IO.h"

#if defined(ARDUINO)

#if defined (__STM32F1__)

// STM32F1 pin definitions, using STM32duino

#if defined(ZUMSPOT_ADF7021)

#define PIN_SCLK       PB5
#define PIN_SREAD      PB6
#define PIN_SDATA      PB7
#define PIN_SLE        PB8
#define PIN_SLE2       PA6
#define PIN_CE         PC14
#define PIN_RXD        PB4
#define PIN_RXD2       PA4
#define PIN_TXD        PB3
#define PIN_TXD2       PA5
#define PIN_CLKOUT     PA15
#define PIN_LED        PC13
#define PIN_DEB        PB9
#define PIN_PTT_LED    PB14
#define PIN_COS_LED    PB15

#elif defined(LIBRE_KIT_ADF7021) || defined(MMDVM_HS_HAT_REV12) || defined(MMDVM_HS_DUAL_HAT_REV10) || defined(NANO_HOTSPOT) || defined(NANO_DV_REV10)

#define PIN_SCLK       PB5
#define PIN_SREAD      PB7
#define PIN_SDATA      PB6
#define PIN_SLE        PB8
#define PIN_SLE2       PA6
#define PIN_CE         PC14
#define PIN_RXD        PB4
#define PIN_RXD2       PA4
#define PIN_TXD        PB3
#define PIN_TXD2       PA5
#define PIN_CLKOUT     PA15
#define PIN_LED        PC13
#define PIN_DEB        PB9
#define PIN_PTT_LED    PB14
#define PIN_COS_LED    PB15

#else
#error "Either ZUMSPOT_ADF7021, LIBRE_KIT_ADF7021, MMDVM_HS_HAT_REV12, MMDVM_HS_DUAL_HAT_REV10, NANO_HOTSPOT or NANO_DV_REV10 need to be defined"
#endif

#elif defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

// Teensy pin definitions
#define PIN_SCLK       2 
#define PIN_SDATA      3
#define PIN_SREAD      4
#define PIN_SLE        5
#define PIN_CE         6
#define PIN_RXD        7
#define PIN_TXD        8
#define PIN_CLKOUT    22
#define PIN_LED       13
#define PIN_DEB       23
#define PIN_PTT_LED   14
#define PIN_COS_LED   15

#else

// Arduino pin definitions (Due and Zero)
#define PIN_SCLK       3 
#define PIN_SDATA      4   // 2 in Arduino Zero Pro
#define PIN_SREAD      5
#define PIN_SLE        6
#define PIN_CE        12
#define PIN_RXD        7
#define PIN_TXD        8
#define PIN_CLKOUT     2   // 4 in Arduino Zero Pro
#define PIN_LED       13
#define PIN_DEB       11
#define PIN_PTT_LED    9
#define PIN_COS_LED   10

#endif

extern "C" {
  void EXT_IRQHandler(void) {
    io.interrupt();
  }
}

void CIO::delay_IFcal() {
  delayMicroseconds(10000);
}

void CIO::delay_reset() {
  delayMicroseconds(300);
}

void CIO::Init()
{
#if defined (__STM32F1__)

#if defined(ZUMSPOT_ADF7021) || defined(LIBRE_KIT_ADF7021) || defined(MMDVM_HS_HAT_REV12) || defined(MMDVM_HS_DUAL_HAT_REV10) || defined(NANO_HOTSPOT) || defined(NANO_DV_REV10)
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);
#endif

#endif
 
  pinMode(PIN_SCLK, OUTPUT);
  pinMode(PIN_SDATA, OUTPUT);
  pinMode(PIN_SREAD, INPUT);
  pinMode(PIN_SLE, OUTPUT);
  pinMode(PIN_CE, OUTPUT);
  pinMode(PIN_RXD, INPUT);
  pinMode(PIN_CLKOUT, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_DEB, OUTPUT);
  pinMode(PIN_PTT_LED, OUTPUT);
  pinMode(PIN_COS_LED, OUTPUT);
  
#if defined(BIDIR_DATA_PIN)
  pinMode(PIN_TXD, INPUT);
#else
  pinMode(PIN_TXD, OUTPUT);
#endif

}

void CIO::startInt()
{
#if defined(BIDIR_DATA_PIN)

// TXD pin is TxRxCLK of ADF7021, standard TX/RX data interface
#if defined (__STM32F1__)
  attachInterrupt(PIN_TXD, EXT_IRQHandler, CHANGE);
#else
  attachInterrupt(digitalPinToInterrupt(PIN_TXD), EXT_IRQHandler, CHANGE);
#endif

#else

#if defined (__STM32F1__)
  attachInterrupt(PIN_CLKOUT, EXT_IRQHandler, CHANGE);
#else
  attachInterrupt(digitalPinToInterrupt(PIN_CLKOUT), EXT_IRQHandler, CHANGE);
#endif

#endif
}

#if defined(BIDIR_DATA_PIN)
// RXD pin is bidirectional in standard interfaces
void CIO::Data_dir_out(bool dir) 
{
  if(dir)
    pinMode(PIN_RXD, OUTPUT);
  else
    pinMode(PIN_RXD, INPUT);
}
#endif

void CIO::SCLK_pin(bool on) 
{
  digitalWrite(PIN_SCLK, on ? HIGH : LOW);
}

void CIO::SDATA_pin(bool on) 
{
  digitalWrite(PIN_SDATA, on ? HIGH : LOW);
}

bool CIO::SREAD_pin()
{
  return digitalRead(PIN_SREAD) == HIGH;
}

void CIO::SLE_pin(bool on) 
{
  digitalWrite(PIN_SLE, on ? HIGH : LOW);
}

void CIO::CE_pin(bool on) 
{
  digitalWrite(PIN_CE, on ? HIGH : LOW);
}

bool CIO::RXD_pin()
{
  return digitalRead(PIN_RXD) == HIGH;
}

bool CIO::CLK_pin()
{
  return digitalRead(PIN_TXD) == HIGH;
}

#if defined(BIDIR_DATA_PIN)
void CIO::RXD_pin_write(bool on)
{
  digitalWrite(PIN_RXD, on ? HIGH : LOW);
}
#endif

void CIO::TXD_pin(bool on) 
{
  digitalWrite(PIN_TXD, on ? HIGH : LOW);
}

void CIO::LED_pin(bool on) 
{
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

void CIO::DEB_pin(bool on) 
{
  digitalWrite(PIN_DEB, on ? HIGH : LOW);
}

void CIO::PTT_pin(bool on) 
{
  digitalWrite(PIN_PTT_LED, on ? HIGH : LOW);
}

void CIO::COS_pin(bool on) 
{
  digitalWrite(PIN_COS_LED, on ? HIGH : LOW);
}

void CIO::delay_us(uint32_t us) {
  ::delayMicroseconds(us);
}
void CIO::dlybit(void)
{
    asm volatile("nop          \n\t"
                 "nop          \n\t"
                 "nop          \n\t"
                 );
}

#endif
