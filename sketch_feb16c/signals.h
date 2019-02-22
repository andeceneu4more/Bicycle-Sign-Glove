#ifndef SIGNALS_H
#define SIGNALS_H

#define LEFT_PIN 4 
#define RIGHT_PIN 7
#define FLASH_RATE 200

long long  lastLeftTime, lastRightTime; 
boolean leftValue, lastLeft, rightValue, lastRight;

void leftSignal()
{
  leftValue = digitalRead(LEFT_PIN);
  if (lastLeft != leftValue)
  {
      lastLeft = leftValue;
      lastLeftTime = millis();
  }
  if (millis() - lastLeftTime > FLASH_RATE)
  {
      leftValue = !leftValue;
      digitalWrite(LEFT_PIN, leftValue);
  }
}

void rightSignal()
{
  rightValue = digitalRead(RIGHT_PIN);
  if (lastRight != rightValue)
  {
      lastRight = rightValue;
      lastRightTime = millis();
  }
  if (millis() - lastRightTime > FLASH_RATE)
  {
      rightValue = !rightValue;
      digitalWrite(RIGHT_PIN, rightValue);
  }
}

void breakLeft()
{
  digitalWrite(LEFT_PIN, LOW);
}

void breakRight()
{
  digitalWrite(RIGHT_PIN, LOW);
}

void setupLights()
{
  pinMode(LEFT_PIN, OUTPUT);
  breakLeft();
  pinMode(RIGHT_PIN, OUTPUT);
  breakRight();
}

#endif
