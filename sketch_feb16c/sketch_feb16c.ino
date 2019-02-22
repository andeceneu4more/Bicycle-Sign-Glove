#include "mpu_gy_521_v2.h"
#include "signals.h"

#define LEFT_STATE -1
#define STRAIGHT_STATE 0
#define RIGHT_STATE 1
#define SIGNAL_TIME 3000

mpu521 myMPU;
int state = 0;
long long lastSignalTime;
boolean signalValue, lastSignal;

void setup() 
{
  Serial.begin(115200);
  myMPU.initialization();
  setupSignals();
}

void loop() 
{ 
  myMPU.recordValues();
  myMPU.printValues();
  switch(state)
  {
      case STRAIGHT_STATE:
          breakLeft();
          breakRight();
          if (myMPU.gyrosY() > Y_MAX && myMPU.gyrosZ() < Z_MIN)
          {
              state = LEFT_STATE;
              signalValue = true;
              lastSignal = false;
          }
          else
            if (myMPU.gyrosY() > Y_MAX && myMPU.gyrosZ() > Z_MAX)
            {
                state = RIGHT_STATE;
                signalValue = true;
                lastSignal = false;
            }
          break;
      case LEFT_STATE:
          leftSignal();
          if (signalValue)
          {
              if (lastSignal != signalValue)
              {
                  lastSignal = signalValue;
                  lastSignalTime = millis();
              }
              if (millis() - lastSignalTime > SIGNAL_TIME)
              {
                  signalValue = !signalValue;
              }
          }
          else
              if (Y_MIN <= myMPU.gyrosY() && myMPU.gyrosY() <= Y_MAX || Z_MIN <= myMPU.gyrosZ() && myMPU.gyrosZ() <= Z_MAX)
              {
                  state = STRAIGHT_STATE;
              }
          break;
      case RIGHT_STATE:
          rightSignal();
          if (signalValue)
          {
              if (lastSignal != signalValue)
              {
                  lastSignal = signalValue;
                  lastSignalTime = millis();
              }
              if (millis() - lastSignalTime > SIGNAL_TIME)
              {
                  signalValue = !signalValue;
              }
          }
          else
              if (Y_MIN <= myMPU.gyrosY() && myMPU.gyrosY() <= Y_MAX || Z_MIN <= myMPU.gyrosZ() && myMPU.gyrosZ() <= Z_MAX)
              {
                  state = STRAIGHT_STATE;
              }
          break;
  }
}
