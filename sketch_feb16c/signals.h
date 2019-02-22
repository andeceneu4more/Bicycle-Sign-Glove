#ifndef SIGNALS_H
#define SIGNALS_H

#define LEFT_PIN 4 
#define RIGHT_PIN 7
#define BUZZER_PIN 9
#define SOUND_FREQ 2000
#define FLASH_RATE 200

long long  lastLeftTime, lastRightTime; 
boolean leftValue, lastLeft, rightValue, lastRight;

void buzzerTone(boolean value)
{
  if (value)
  {
      tone(BUZZER_PIN, SOUND_FREQ);
  }
  else
  {
      noTone(BUZZER_PIN);
  }
}

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
      buzzerTone(leftValue);
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
      buzzerTone(rightValue);
  }
}

void breakLeft()
{
  digitalWrite(LEFT_PIN, LOW);
  noTone(BUZZER_PIN);
}

void breakRight()
{
  digitalWrite(RIGHT_PIN, LOW);
  noTone(BUZZER_PIN);
}

void setupSignals()
{
  pinMode(LEFT_PIN, OUTPUT);
  breakLeft();
  pinMode(RIGHT_PIN, OUTPUT);
  breakRight();
  pinMode(BUZZER_PIN, OUTPUT);
}

#endif
