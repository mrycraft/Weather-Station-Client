#define BMP280 // or BMP085

#include "Camera.h"
#include "BMPSensor.h"
#include "DataClient.h"
#include "Credentials.h"

// Update these credentials in Credentials.h
DataClient client(SSID, PASSWORD, HOST_NAME, PORT, DEVICE_NAME, false);
Camera camera;
BMPSensor bmp;

unsigned long imageInterval = 30000;
unsigned long bmpInterval = 10000;

unsigned long lastImageTime = 0;
unsigned long lastBmpTime = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(100);

    client.Connect();
    bmp.Begin(33, 32);
    camera.Init();

    lastImageTime = millis();
    lastBmpTime = millis();
}

void loop()
{
    unsigned long currentTime = millis();

    // Reconnect if disconnected
    if(!client.IsConnected())
    {
        Serial.println("Disconnected, reconnecting...");
        client.Connect();

        lastImageTime = currentTime;
        lastBmpTime = currentTime;
    }

    // Send image every interval
    if (currentTime - lastImageTime >= imageInterval)
    {
        size_t bufferLength = 0;
        uint8_t* buffer = camera.TakeImage(&bufferLength);
        client.SendData(buffer, bufferLength, DataType::Image);

        Serial.println("Sent image");

        lastImageTime = currentTime;
    }


    // Send BMP data every interval
    if (currentTime - lastBmpTime >= bmpInterval)
    {
        float temperature = bmp.ReadTemperature();
        float pressure = bmp.ReadPressure();

        client.SendData(temperature, DataType::Temperature);
        client.SendData(pressure, DataType::Pressure);

        Serial.println("Sent BMP data");

        lastBmpTime = currentTime;
    }
}

