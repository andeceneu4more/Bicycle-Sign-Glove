#include "mpu_gy_521.h"
#include "signals.h"

mpu521 myMPU;

void setup() 
{
  Serial.begin(9600);
  myMPU.initialization();
  // setupLights();
}

void loop() 
{
  myMPU.recordValues();
  myMPU.printBoundaries();
}
