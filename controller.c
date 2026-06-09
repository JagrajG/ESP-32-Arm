#include <esp_now.h>
#include <WiFi.h>

// Potentiometer pins
const int potPins[4] = {34, 35, 32, 33};

// Replace this with your receiver ESP32 MAC address
uint8_t receiverAddress[] = {0xA0, 0xB7, 0x65, 0x12, 0x34, 0x56};

typedef struct ArmPacket
{
    int pot1;
    int pot2;
    int pot3;
    int pot4;
} ArmPacket;

ArmPacket dataToSend;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("Send Status: ");

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        Serial.println("Success");
    }
    else
    {
        Serial.println("Fail");
    }
}

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    analogReadResolution(12); // ESP32 ADC: 0 to 4095

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add receiver peer");
        return;
    }

    Serial.println("Controller ready");
}

void loop()
{
    dataToSend.pot1 = analogRead(potPins[0]);
    dataToSend.pot2 = analogRead(potPins[1]);
    dataToSend.pot3 = analogRead(potPins[2]);
    dataToSend.pot4 = analogRead(potPins[3]);

    esp_err_t result = esp_now_send(
        receiverAddress,
        (uint8_t *)&dataToSend,
        sizeof(dataToSend));

    if (result == ESP_OK)
    {
        Serial.print("Sent: ");
        Serial.print(dataToSend.pot1);
        Serial.print(" ");
        Serial.print(dataToSend.pot2);
        Serial.print(" ");
        Serial.print(dataToSend.pot3);
        Serial.print(" ");
        Serial.println(dataToSend.pot4);
    }
    else
    {
        Serial.println("Send error");
    }

    delay(30);
}