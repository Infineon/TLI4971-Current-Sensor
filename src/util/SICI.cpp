/**
 * SICI.cpp - Part of the library for Arduino to control the tli4971-D050T4 current sensor.
 *
 * tli4971 is a high-precision current sensor based on Infineon´s proven Hall technology. 
 * The coreless concept significantly reduces footprint compared with existing solutions. 
 * tli4971 is an easy-to-use, fully digital solution that does not require external calibration 
 * or additional parts such as A/D converters, 0 pAmps or reference voltage. 
 * 
 * Have a look at the application note/reference manual for more information.
 * 
 * This file uses the OneWire library, originally developed by Jim Studt and many contributors since then.
 * Please have a look at the OneWire library for additional information about the license, copyright, and contributors.
 * 
 * Copyright (c) 2018 Infineon Technologies AG
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   
 */

#include "SICI.h"

#define T_EN 150
#define T_EN_MAX 400
#define T_LOW 20
#define ENTER_IF_COMMAND 0xDCBA

tli4971::Sici::Sici(uint8_t pin, uint8_t pwrPin)
{
	mActive = false;
	mInterface = nullptr;
	mPin = pin;
	mPwrPin = pwrPin;
}

void tli4971::Sici::begin(void)
{
  #if defined(XMC1100_XMC2GO) || defined(XMC1100_Boot_Kit) || defined(XMC4700_Relax_Kit)
	onewire::Timing_t timingTLI4971 = { 40, 80, 60, 120, 6000 };
	mInterface = new OneWire(mPin, &timingTLI4971);
  #else
	mInterface = new OneWire(mPin);
  #endif
	mActive = true;
}

void tli4971::Sici::end(void)
{
	digitalWrite(mPin, LOW);
	delay(6);	//End SICI communication
	pinMode(mPin, INPUT);
	delete mInterface;
	mInterface = nullptr;
	mActive = false;
}

bool tli4971::Sici::enterSensorIF()
{
  uint16_t rec;
  //restart Sensor by switching VDD off and on again
  digitalWrite(mPwrPin, HIGH);		//For green Shield: digitalWrite(mPwrPin, LOW);
  digitalWrite(mPin, LOW);
  delay(100);
  digitalWrite(mPwrPin, LOW);		//For green Shield: digitalWrite(mPwrPin, HIGH);
  //+IFX_ONEWIRE_PINOUTHIGH;
  //send low pulse to activate Interface
  delayMicroseconds(T_EN + T_LOW);
  digitalWrite(mPin, HIGH);
  delayMicroseconds(T_EN_MAX-T_EN-T_LOW);
  //send enter-interface-command
  rec = transfer16(ENTER_IF_COMMAND);
  
  return rec == 0;
}

uint16_t tli4971::Sici::transfer16(uint16_t dataIn)
{
	uint16_t dataOut = 0;
  //transfer and read data with LSB first
  for(int i = 0; i < 16; i++)
  {
    write_bit((dataIn>>(i))&0x1);
    dataOut |= (read_bit()&0x1)<<(i);
  }
  return dataOut;
}

uint8_t tli4971::Sici::read_bit(void)
{
	// 1 is a long low pulse, 0 is a short low pulse
	// exactly different from onewire, so we have to invert it
	return !(mInterface->read_bit());
}

void tli4971::Sici::write_bit(uint8_t value)
{
	// 1 is a long low pulse, 0 is a short low pulse
	// exactly different from onewire, so we have to invert it
	mInterface->write_bit(!value);
}
