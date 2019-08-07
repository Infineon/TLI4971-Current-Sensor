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

#define mapThrS1(thr) (thr<8)?0x16+thr*3+(int)(thr*0.25):0x10+(thr-8)*5-(int)((thr-8)*0.25)
#define mapThrS2(thr) (thr<8)?0x11+thr*3-(int)(thr/3):0x0D+(thr-8)*4-(int)((thr-8)/6)
#define mapThrS3(thr) (thr<8)?0xB+thr*2:0x08+(thr-8)*3
#define mapThrS4(thr) (thr<8)?0x17+thr*3+(int)((thr+1)/3):0x11+(thr-8)*5
#define mapThrS5(thr) (thr<8)?0x10+thr*2+(int)(thr*0.5):0xB+(thr-8)*4-(int)((thr-8)*0.25)
#define mapThrS6(thr) (thr<8)?0x9+thr+(int)(thr*0.5)+(int)(thr>2 && thr%2):0x06+(thr-8)*2+(int)((thr-8)*0.5)

#define NO_HYST_THR -1
#define mapHystThrS1(thr) (thr<8)?0x5+thr-(int)((thr+2)/3):0x4+(thr-8)-(int)((thr-8)*0.25)
#define mapHystThrS2(thr) (thr<8)?0x4+thr-(int)(thr/3)-(int)((thr+7)/8):0x3+(thr-8)-(int)((thr-7)/4)
#define mapHystThrS3(thr) (thr<8)?0x3+(thr>=3)+(thr>=5):0x2+(thr-8)-(int)((thr-8)/2)+int((thr-8)/6)-((thr-8)>6)
#define mapHystThrS4(thr) (thr<8)?0x5+thr-(int)((thr+1)/3):0x4+(thr-8)
#define mapHystThrS5(thr) (thr<8)?0x4+thr-(int)((thr+1)/2):0x3+(thr-8)-(int)((thr-5)/4)
#define mapHystThrS6(thr) (thr<8)?0x2+(int)((thr+1)/3):0x2+(int)((thr-8)/2)

/**
 * @class TLI4971
 */
class TLI4971
{
  public:

    enum OCDMODE { NONE, INTERRUPT, POLLING };
    
    enum MEASRANGE { FSR120 = 0x05, FSR100 = 0x06, FSR75 = 0x08, FSR50 = 0x0C, FSR37_5 = 0x10, FSR25 = 0x18 };
    enum OPMODE { SD_BID = 0, FD = 0x1<<5 , SD_UNI = 0x2<<5, SE = 0x3<<5 }; 
    enum OCDDEGLITCH { D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15 };
    enum OCDTHR { THR1_1, THR1_2, THR1_3, THR1_4, THR1_5, THR1_6, THR1_7, THR1_8, THR2_1, THR2_2, THR2_3, THR2_4, THR2_5, THR2_6, THR2_7, THR2_8 };
    enum VRefExt { V1_65, V1_2, V1_5, V1_8, V2_5 };

    TLI4971(int aout, int vref, int pwr, int sici, int ocd1, int ocd2, int mux, bool xmc5V = true);
    ~TLI4971(void);
    bool begin(void);
    bool reset(void);
    void end(void);
    double read(void);
    void ocdPolling(void);

    void configAdc(bool logicLevel5V, int adcResolution);
    
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
    int adcResol = 12;

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
