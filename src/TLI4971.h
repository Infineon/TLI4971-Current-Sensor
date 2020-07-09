/** 
 * @file        TLI4971.h
 * @brief       TLI4971 C++ Class
 * @date        August 2019
 * @copyright   Copyright (c) 2019 Infineon Technologies AG
 */

#ifndef TLI4971_H_INCLUDED
#define TLI4971_H_INCLUDED

#include <Arduino.h>
#include <stdlib.h>
#include <stdbool.h>
#include "util/SICI.h"


/**
 * @class TLI4971
 */
class TLI4971
{
  public:

    enum OCDMODE { NONE, INTERRUPT, POLLING };
    
    enum MEASRANGE { FSR120 = 0x05, FSR100 = 0x06, FSR75 = 0x08, FSR50 = 0x0C, FSR37_5 = 0x10, FSR25 = 0x18 };
    enum OPMODE { SD_BID = 0, FD = 0x1<<5 , SD_UNI = 0x2<<5, S_ENDED = 0x3<<5 }; 
    enum OCDDEGLITCH { D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15 };
    enum OCDTHR { THR1_1, THR1_2, THR1_3, THR1_4, THR1_5, THR1_6, THR1_7, THR1_8, THR2_1, THR2_2, THR2_3, THR2_4, THR2_5, THR2_6, THR2_7, THR2_8 };
    enum VRefExt { V1_65, V1_2, V1_5, V1_8, V2_5 };

    TLI4971(int aout, int vref, int pwr, int sici, int ocd1, int ocd2, int mux, bool mc5V = true);
    ~TLI4971(void);
    bool begin(void);
    bool reset(void);
    void end(void);
    double read(void);
    void ocdPolling(void);

    void configAdc(bool logicLevel5V, int adcResolution = -1);
    
    double getLastSwOcdCurrent(void);

    bool registerOcd1Function(int mode, void (*func)(void));
    bool registerOcd2Function(int mode, void (*func)(void));
    bool registerSwOcdFunction(double currentLevel, void (*func)(void));

    bool setMeasRange(int measuringRange);
    bool setOpMode(int operatingMode);
    bool configOcd1(bool enable, int threshold = THR1_1, int deglitchTime = D0);
    bool configOcd2(bool enable, int threshold = THR2_1, int deglitchTime = D0);
    bool setOcdCompHyst(int threshold);
    bool setSwOcdCompHyst(double hysterese);
    bool setVrefExt(int vrefExtVoltage);
	bool setRatioGain(bool enable);
	bool setRatioOff(bool enable);

    bool getOcd1State(void);
    bool getOcd2State(void);
    bool getSwOcdState(void);

  
    
  private:
    //const Sici_TLI4971::_SICI_timing SICI_timing = {150,400,60,120,120,60,16,120,35,20,20};
    tli4971::Sici bus = tli4971::Sici(0,0);
    int ocd1Pin;
    int ocd2Pin;
    int pwrPin;
    int vrefPin;
    int aoutPin;
    int siciPin;
    int muxPin;

    int ocd1FuncMode;
    int ocd2FuncMode;

    int measRange;
    int opMode;
    int ocd1Mode;
    int ocd2Mode;
    int vrefExt;

    bool ll5V = true;
#ifdef ADC_RESOLUTION
	int adcResol = ADC_RESOLUTION //if possible: highest possible resolution
#else
    int adcResol = 10;	//standard for Arduino UNO
#endif

    uint16_t configRegs[3];

    double swOcdThreshold = 0;  //Alarm triggered if current exceeds this level [A]
    double lastSwOcdValue = 0;
    double swOcdCompHyst = 0;
    bool swOcdTriggered = false;
    bool lastOcd1PinState = HIGH;
    bool lastOcd2PinState = HIGH;
    
    void (*_ocd1Function)(void);
    void (*_ocd2Function)(void);
    void (*_swOcdFunction)(void);
    
    bool sendConfig(void);
	
};

#endif
