#include "Wire.h"

#if defined(BMP280)
#include "Adafruit_BMP280.h"
#else if defined(BMP085)
#include "Adafruit_BMP085.h"
#endif

class BMPSensor
{
public:

#if defined(BMP280)
    void Begin(int sda=33, int scl=32, bool forceMeasurements=false)
    {
        this->forceMeasurements = forceMeasurements;
        wire.setPins(sda, scl);
        bmp = Adafruit_BMP280(&wire);

        unsigned status;
        status = bmp.begin(0x76);
        while (!status)
        {
            Serial.println(F("Could not find a valid BMP280 sensor"));
            Serial.print(F("SensorID was: 0x"));
            Serial.println(bmp.sensorID(), 16);
            delay(1000);
            status = bmp.begin(0x76);
        }

        /* Default settings from datasheet. */
        bmp.setSampling(forceMeasurements ? Adafruit_BMP280::MODE_FORCED : Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

        Serial.println(F("BMP280 sensor found!"));
        
    }
#else if defined(BMP085)

    void Begin(int sda=33, int scl=32, bool forceMeasurements=false)
    {
        this->forceMeasurements = forceMeasurements;
        wire.setPins(sda, scl);
        bmp = Adafruit_BMP085();

        unsigned status;
        status = bmp.begin(BMP085_ULTRAHIGHRES, &wire);
        while (!status)
        {
            Serial.println(F("Could not find a valid BMP085 sensor"));
            delay(1000);
            status = bmp.begin(BMP085_ULTRAHIGHRES, &wire);
        }

        Serial.println(F("BMP085 sensor found!"));
    }

#endif

float ReadTemperature()
{
#if defined(BMP280)
    
    if (forceMeasurements)
    {
        Serial.println("Forcing measurement Temp");
        if(bmp.takeForcedMeasurement())
        {
            Serial.println("Taking measurement Temp");
            return bmp.readTemperature();
        }

        return 0;
    }

    return bmp.readTemperature();

#else if defined(BMP085)

    return bmp.readTemperature();

#endif
}

float ReadPressure()
{
#if defined(BMP280)

    if (forceMeasurements)
    {
        Serial.println("Forcing measurement Pressure");
        if(bmp.takeForcedMeasurement())
        {
            Serial.println("Taking measurement Pressure");
            return bmp.readPressure();
        }

        return 0;
    }

    return bmp.readPressure();
    
#else if defined(BMP085)
    
    return bmp.readPressure();

#endif
}


private:
    TwoWire wire = TwoWire(2);

#if defined(BMP280)
    Adafruit_BMP280 bmp;
#else if defined(BMP085)
    Adafruit_BMP085 bmp;
#endif

    int sda;
    int scl;
    bool forceMeasurements;
};
