#include <ESP32Servo.h>

// Potentiometer pins
const int potPins[4] = {34, 35, 32, 33};

// Servo signal pins
const int servoPins[4] = {13, 14, 26, 27};

// Servo objects
Servo servos[4];

// Servo angle limits
// Adjust these later to protect your robotic arm
const int servoMinAngle[4] = {0, 0, 0, 0};
const int servoMaxAngle[4] = {180, 180, 180, 180};

// Smoothing values
int smoothedPotValues[4] = {0, 0, 0, 0};

// Higher = smoother but slower response
const float smoothingFactor = 0.15;

void setup()
{
    Serial.begin(115200);

    // ESP32 ADC range setup
    analogReadResolution(12); // 0 to 4095

    // Attach servos
    for (int i = 0; i < 4; i++)
    {
        servos[i].setPeriodHertz(50);              // Standard servo frequency
        servos[i].attach(servoPins[i], 500, 2400); // pulse width range in microseconds
    }

    // Initialize smoothing values
    for (int i = 0; i < 4; i++)
    {
        smoothedPotValues[i] = analogRead(potPins[i]);
    }

    Serial.println("4 Potentiometer + 4 Servo Robotic Arm Started");
}

void loop()
{
    for (int i = 0; i < 4; i++)
    {
        int rawPotValue = analogRead(potPins[i]);

        // Smooth the potentiometer reading
        smoothedPotValues[i] =
            (smoothingFactor * rawPotValue) +
            ((1.0 - smoothingFactor) * smoothedPotValues[i]);

        // Convert 0-4095 analog value to servo angle
        int angle = map(
            smoothedPotValues[i],
            0,
            4095,
            servoMinAngle[i],
            servoMaxAngle[i]);

        // Safety clamp
        angle = constrain(angle, servoMinAngle[i], servoMaxAngle[i]);

        servos[i].write(angle);

        Serial.print("Servo ");
        Serial.print(i + 1);
        Serial.print(" | Pot: ");
        Serial.print(rawPotValue);
        Serial.print(" | Angle: ");
        Serial.print(angle);
        Serial.print("    ");
    }

    Serial.println();

    delay(20);
}