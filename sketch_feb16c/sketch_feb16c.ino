#include "mpu_gy_521_v2.h"
#include "signals.h"

#define LEFT_STATE -1
#define STRAIGHT_STATE 0
#define RIGHT_STATE 1

mpu521 myMPU;
void setup() 
{
  Serial.begin(115200);
  myMPU.initialization();
  setupLights();
}
int state = 0;
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
          }
          else
            if (myMPU.gyrosY() > Y_MAX && myMPU.gyrosZ() > Z_MAX)
            {
                state = RIGHT_STATE;
            }
          break;
      case LEFT_STATE:
          leftSignal();
          if (Y_MIN <= myMPU.gyrosY() && myMPU.gyrosY() <= Y_MAX || Z_MIN <= myMPU.gyrosZ() && myMPU.gyrosZ() <= Z_MAX)
          {
              state = STRAIGHT_STATE;
          }
          break;
      case RIGHT_STATE:
          rightSignal();
          if (Y_MIN <= myMPU.gyrosY() && myMPU.gyrosY() <= Y_MAX || Z_MIN <= myMPU.gyrosZ() && myMPU.gyrosZ() <= Z_MAX)
          {
              state = STRAIGHT_STATE;
          }
          break;
  }
}
/*
switch(state)
  {
      case STRAIGHT_STATE:
          breakLeft();
          breakRight();
          if (myMPU.gyrosX() < X_MIN && myMPU.gyrosZ() < Z_MIN)
          {
              state = LEFT_STATE;
          }
          else
            if (myMPU.gyrosX() > X_MAX && myMPU.gyrosZ() > Z_MAX)
            {
                state = RIGHT_STATE;
            }
          break;
      case LEFT_STATE:
          leftSignal();
          if (X_MIN <= myMPU.gyrosX() && myMPU.gyrosX() <= X_MAX || Z_MIN <= myMPU.gyrosZ() && myMPU.gyrosZ() <= Z_MAX)
          {
              state = STRAIGHT_STATE;
          }
          break;
      case RIGHT_STATE:
          rightSignal();
          if (X_MIN <= myMPU.gyrosX() && myMPU.gyrosX() <= X_MAX || Z_MIN <= myMPU.gyrosZ() && myMPU.gyrosZ() <= Z_MAX)
          {
              state = STRAIGHT_STATE;
          }
          break;
  }
 * /
 */
