#include <TLI4971.h>

#if defined(XMC1100_XMC2GO) 
  TLI4971 CurrentSensor = TLI4971(A0, A1, 5, 8, 9, 3, 4, false);   //XMC2Go
#elif defined(XMC1100_Boot_Kit) || defined(XMC4700_Relax_Kit) || defined(__AVR_ATmega328P__) /**< MyIoT Adapter Socket 1 */
  TLI4971 CurrentSensor = TLI4971(A0, A1, 9, 5, 2, 10, 20, true);     
#endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1000000);
  delay(1000);

  while(!CurrentSensor.begin())
  {
    Serial.println("Current Sensor init failed!");
    delay(100);
  }
  CurrentSensor.setMeasRange(TLI4971::FSR25);
  CurrentSensor.setOpMode(TLI4971::SD_BID);
  /*CurrentSensor.registerSwOcdFunction(12.5,ocd_func);
  CurrentSensor.setSwOcdCompHyst(5.0);*/
  CurrentSensor.registerOcd1Function(CurrentSensor.INTERRUPT,ocd1_func);
  CurrentSensor.configOcd1(true,CurrentSensor.THR1_8);
  CurrentSensor.setOcdCompHyst(CurrentSensor.THR1_1);
}

void loop() {
  // put your main code here, to run repeatedly:
  long timer = millis();
  int numReads = 0;
  double avg = 0.0;
  while(millis()-timer < 500)
  {
    avg += CurrentSensor.read(); 
    numReads++;
  }
  avg /= numReads;
  //Serial.print("Reads: ");
  //Serial.print(numReads);
  //Serial.print(",\t");
  Serial.println(avg);
}

void ocd_func()
{
  Serial.println();
  Serial.print("OCD: ");
  Serial.println(CurrentSensor.getLastSwOcdCurrent());
  Serial.println();
}

void ocd1_func()
{
  Serial.println();
  Serial.println("OCD1");
  Serial.println();
}
