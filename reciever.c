#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// Servo signal pins
const int servoPins[4] = {13, 14, 26, 27};

Servo servos[4];

typedef struct ArmPacket
{
    int pot1;
    int pot2;
    int pot3;
    int pot4;
} ArmPacket;

ArmPacket receivedData;

// Adjust these to protect your robotic arm joints
const int servoMinAngle[4] = {0, 0, 0, 0};
const int servoMaxAngle[4] = {180, 180, 180, 180};

// Optional smoothing
float smoothedAngles[4] = {90, 90, 90, 90};
const float smoothingFactor = 0.25;

void onDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));

    int potValues[4] = {
        receivedData.pot1,
        receivedData.pot2,
        receivedData.pot3,
        receivedData.pot4};

    for (int i = 0; i < 4; i++)
    {
        int targetAngle = map(
            potValues[i],
            0,
            4095,
            servoMinAngle[i],
            servoMaxAngle[i]);

        targetAngle = constrain(
            targetAngle,
            servoMinAngle[i],
            servoMaxAngle[i]);

        smoothedAngles[i] =
            smoothingFactor * targetAngle +
            (1.0 - smoothingFactor) * smoothedAngles[i];

        servos[i].write((int)smoothedAngles[i]);
    }

    Serial.print("Received: ");
    Serial.print(receivedData.pot1);
    Serial.print(" ");
    Serial.print(receivedData.pot2);
    Serial.print(" ");
    Serial.print(receivedData.pot3);
    Serial.print(" ");
    Serial.println(receivedData.pot4);
}

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    for (int i = 0; i < 4; i++)
    {
        servos[i].setPeriodHertz(50);
        servos[i].attach(servoPins[i], 500, 2400);
        servos[i].write(90);
    }

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_recv_cb(onDataReceived);

    Serial.println("Receiver ready");
}

void loop()
{
    // Nothing needed here.
    // Servos update when ESP-NOW data is received.
}