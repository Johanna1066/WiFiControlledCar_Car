/*
 * steeringFuncions.cpp
 */
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <vector>

#include "steeringFunctions.h"
#include "Sensors/usSensor.h"
#include "Motors/engine.h"
#include "Motors/steeringServo.h"
#include "globalVariables.h"

/**
 * @brief Sets the velocity and direction of all engine objects.
 *
 * @param velocity The velocity to set for the engines.
 * @param obstacle Indicates if there is an obstacle (true if obstacle present, false otherwise).
 */
void setEnginesVelocity(int velocity, bool obsticle)
{
    for (auto &engine : globalVariables::engines)
    {
        engine.setVelocity(velocity, obsticle);
    }
}

/**
 * @brief Handles incoming data from the controller ESP32.
 *
 * @param mac The MAC address of the sender.
 * @param incomingData The incoming data received.
 * @param len The length of the incoming data.
 */
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&globalVariables::dataRecieved, incomingData, sizeof(globalVariables::dataRecieved));

    if (globalVariables::dataRecieved >= 10000)
    {
        if (xSemaphoreTake(globalVariables::servoHandle, 100) == pdTRUE)
        {
            /**
             * 10000 is added to the data sent from the controller if the data
             * is supposed to be a servo command, to distinguish from engine commands.
             * Therefore the data has to be modified before sending the actual
             * command to the servo object.
             */
            globalVariables::dataRecieved -= 10000;

            globalVariables::servo.setDirection(globalVariables::dataRecieved);
            delay(1);
            xSemaphoreGive(globalVariables::servoHandle);
        }
    }
    else if ((globalVariables::dataRecieved >= 0) && (globalVariables::dataRecieved <= 4096))
    {
        if (xSemaphoreTake(globalVariables::engineHandle, 100) == pdTRUE)
        {

            setEnginesVelocity(globalVariables::dataRecieved, globalVariables::hinderForwardMovement);
            delay(1);
            xSemaphoreGive(globalVariables::engineHandle);
        }
    }
}

/**
 * @brief Initializes hardware and communication protocols.
 *
 */
void initiate()
{
    globalVariables::servo.initiateServo();

    globalVariables::engines.push_back(globalVariables::leftEngine);
    globalVariables::engines.push_back(globalVariables::rightEngine);

    for (auto &engine : globalVariables::engines)
    {
        engine.intitateEngine();
    }

    globalVariables::sensor.initiateUSsensor();

    globalVariables::engineHandle = xSemaphoreCreateMutex();
    globalVariables::servoHandle = xSemaphoreCreateMutex();

    if (globalVariables::engineHandle == NULL)
    {
        Serial.println("Error creating engine semaphore");
        return;
    }
    if (globalVariables::servoHandle == NULL)
    {
        Serial.println("Error creating servo semaphore");
        return;
    }

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(onDataRecv);
}

/**
 * @brief Checks the sensor reading and updates the movement flag.
 *
 * @param parameters Additional parameters for the function (typically unused).
 */
void sensorCheck(void *parameters)
{
    for (;;)
    {
        globalVariables::sensor.USsensor::readDistance();
        globalVariables::reading = globalVariables::sensor.USsensor::getDistance();

        if (globalVariables::reading < globalVariables::safeServoDistance)
        {
            if (xSemaphoreTake(globalVariables::engineHandle, 100) == pdTRUE)
            {
                if (!globalVariables::hinderForwardMovement)
                {
                    globalVariables::hinderForwardMovement = true;
                }
                xSemaphoreGive(globalVariables::engineHandle);
            }
        }
        else
        {
            if (xSemaphoreTake(globalVariables::engineHandle, 100) == pdTRUE)
            {
                if (globalVariables::hinderForwardMovement)
                {
                    globalVariables::hinderForwardMovement = false;

                }
                xSemaphoreGive(globalVariables::engineHandle);
            }
        }
        vTaskDelay(10);
    }
}