#include <Arduino.h>
#include <iostream>
#include <vector>



#include "Motors/Engine.h"
#include "Motors/SteeringServo.h"
#include "Sensors/Joystick.h"
#include "Sensors/USsensor.h"
#include "steeringFunctions.h"

void stopEngines();
void setEnginesVelocity(int);
void initiate(); 

void readSensor(void *parameters)
{
  for (;;)
  {
    mySensor.taskReadDistance();
    // if case here
    if (mySensor.getDistance() < 20)
    {
      stopEngines();
      Serial.println("STOP!");
    }
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    //Send radio Signal here
  }
}
/*
void printSensorReading(void *parameters)
{
  for (;;)
  {
      reading = mySensor.getDistance();
      Serial.println(reading);
      vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}*/


void setup()
{
  initiate();
  Serial.begin(9600);

  xTaskCreate(
      readSensor,    // Function name
      "*readSensor", // Task name
      1000,          // Stack size
      NULL,          // Task parameters
      1,             // Task priority
      NULL           // Task handle
  );
/*
  xTaskCreate(
      printSensorReading,    // Function name
      "*printSensorReading", // Task name
      1000,                  // Stack size
      NULL,                  // Task parameters
      1,                     // Task priority
      NULL                   // Task handle
  );*/
}

void loop()
{
  
}
