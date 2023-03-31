#include <WiFi.h>
#include <WiFiClient.h>

enum DataType : uint8_t
{
    Image = 0,
    Temperature = 1,
    Humidity = 2,
    Pressure = 3,
    Name = 4,
    Other = 5
};

class DataClient
{
public:
    DataClient(const char* ssid, const char* password, const char* host, uint16_t port, const char* deviceName, bool autoConnect = true);
    ~DataClient();
    void Connect(const char* host, uint16_t port);
    void Connect();
    void Disconnect();
    bool IsConnected();
    void SetDeviceName(const char* deviceName);
    void SetVerbose(bool verbose);
    
    void SendData(const char *data, DataType type);
    void SendData(uint8_t* data, size_t bufferLength, DataType type);
    void SendData(float data, DataType type);
    void SendData(int data, DataType type);

private:
    const char *_ssid;
    const char *_password;
    const char *_host;
    uint16_t _port;
    const char *_deviceName;
    WiFiClient _client;
    bool _verbose = false;
};