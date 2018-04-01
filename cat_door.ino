#include <Servo.h>

Servo servo;

const int sensorPin = 0;
const int buttonPin = 2;

long startTime;
const long interval = 300000; //5 min * 60 s/min * 1000 ms/s

int numIncreases;
const int requiredNumIncreases = 4;

int lightLevel;
int oldLightLevel;
const int requiredLightIncrease = 3;
const int requiredLightLevel = 35;

bool waitingPeriod;
const long waitingTime = 1800000;//30 min * 60 s/min * 1000 ms/s

bool buttonPressed;
bool isClosed;

void setup()
{ 
  Serial.begin(9600);
  Serial.println("Started");  
  
  servo.attach(9);
  pinMode(buttonPin, INPUT);

  setState(false);

  updateButtonPressed();
}

void loop()
{ 
  bool oldButtonPressed = buttonPressed;
  updateButtonPressed();
  if (buttonPressed && !oldButtonPressed) //button just got pressed
  {
    setState(!isClosed);
  }
  else if (isClosed)
  {
    if (waitingPeriod)
    {
      if (millis() - startTime >= waitingTime)
      {
        Serial.println("waiting done");
        setState(false);
      }
    }
    else if (numIncreases >= requiredNumIncreases) //should only get to requiredNumIncreases
    {
      updateLightLevel();
      if (lightLevel >= requiredLightLevel)
      {
        resetStartTime();
        waitingPeriod = true;
        Serial.println("waiting");
      }
    }
    else
    {
      if (millis() - startTime >= interval)
      {
        oldLightLevel = lightLevel;
        updateLightLevel();
        if (lightLevel - oldLightLevel >= requiredLightIncrease)
        {
          numIncreases++;
          Serial.print("num increases: "); Serial.println(numIncreases);
        }
        resetStartTime();
      }
    }
  }
  delay(100);
}

void setState(bool newIsClosed)
{
  if(newIsClosed)
  {
    servo.write(135);
    Serial.println("close");

    waitingPeriod = false;
    numIncreases = 0;
    resetStartTime();
    updateLightLevel();
  }
  else
  {
    servo.write(45);
    Serial.println("open");
  }
  isClosed = newIsClosed;
}

void updateLightLevel()
{
  lightLevel = map(analogRead(sensorPin), 0, 1023, 100, 0);
  Serial.print("light level: "); Serial.println(lightLevel);
}

void updateButtonPressed()
{
  buttonPressed = !digitalRead(buttonPin);
}

void resetStartTime()
{
  startTime = millis();
}
