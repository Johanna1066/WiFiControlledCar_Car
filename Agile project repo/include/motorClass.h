#pragma once

class Motor
{
private:
public:
    void forwardData();
};

void Motor::forwardData()
{
}

//----------Engine class---------------

class Engine : public Motor
{
private:
    int engineVelocityPIN{};
    int enginePositivePIN{};
    int engineNegativePIN{};

    int velocity{};
    bool direction{}; // true = forward, false = backwards

public:
    Engine::Engine(int velocityPIN, int positivePIN, int negativePIN)
    {
        engineVelocityPIN = velocityPIN;
        enginePositivePIN = positivePIN;
        engineNegativePIN = negativePIN;
    }

    Engine::~Engine()
    {
        delete engineVelocityPIN;
        delete enginePositivePIN;
        delete engineNegativePIN;

        delete velocity;
        delete direction;
    }

    void setVelocity(int newVelocity);
    {
        analogWrite(engineVelocityPIN, newVelocity);
    }

    void intitateEngine()
    {
        pinMode(engineVelocity, OUTPUT);
        pinMode(enginePositive, OUTPUT);
        pinMode(engineNegative, OUTPUT);
    }
    void directionReverse()
    {
        // May change depending on connection to enginedriver hardware
        // IDEA: add if- depending on bool diretion.

        digitalWrite(enginePositive, LOW);
        digitalWrite(engineNegative, HIGH);
    }
};

//---------Servo class---------------

class Servo : public Motor
{
private:
    // TODO: Add PINs
    int direction{};

public:
    Servo::Servo()
    {
    }

    Servo::~Servo()
    {
        delete direction;
    }

    void setDirection(int newDirection)
    {
        // IDEA: Decide velocity by moving the pointer in tiny steps with delay??
    }
};
