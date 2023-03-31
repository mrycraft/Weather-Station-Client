#include "DataClient.h"

DataClient::DataClient(const char* ssid, const char* password, const char* host, uint16_t port, const char* deviceName, bool autoConnect)
{
    _ssid = ssid;
    _password = password;
    _host = host;
    _port = port;
    _deviceName = deviceName;
    
    if(autoConnect) Connect();
}

DataClient::~DataClient()
{
}

void DataClient::Connect(const char* host, uint16_t port)
{
    _host = host;
    _port = port;

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(_ssid, _password);
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\n\nConnected to ");
    Serial.println(_ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());

    Serial.printf("Connecting to %s:%d", _host, _port);

    while (!IsConnected())
    {
        Serial.print(".");
        _client.connect(_host, _port);
        delay(1000);
    }

    // send device name
    SetDeviceName(_deviceName);
    delay(100);

    Serial.println("Connected!\n");
}

void DataClient::Connect()
{
    Connect(_host, _port);
}

void DataClient::Disconnect()
{
    _client.stop();
}

bool DataClient::IsConnected()
{
    return _client.connected();
}

void DataClient::SetDeviceName(const char* deviceName)
{
    _deviceName = deviceName;
    SendData(_deviceName, DataType::Name);
}

void DataClient::SetVerbose(bool verbose)
{
    _verbose = verbose;
}

void DataClient::SendData(uint8_t* data, size_t bufferLength, DataType type)
{
    if(_verbose) Serial.println("Sending data...");

    uint8_t* fullMessage = (uint8_t*)malloc(bufferLength + 9);

    if(_verbose) Serial.println("Created full message buffer");

    uint64_t length = bufferLength;
    memcpy(fullMessage, &length, 8);

    if(_verbose) Serial.print("Copied length:");
    if(_verbose) Serial.println(length);

    memcpy(fullMessage + 8, &type, 1);

    if(_verbose) Serial.println("Copied type");

    memcpy(fullMessage + 9, data, bufferLength);

    if(_verbose) Serial.println("Copied data");

    size_t sendByteCount = _client.write(fullMessage, bufferLength + 9);

    if(_verbose) Serial.printf("Sent %d bytes out of %d bytes\n", sendByteCount, bufferLength + 9);

    _client.flush();

    free(fullMessage);
}

void DataClient::SendData(float data, DataType type)
{
    // convert to bytes
    uint8_t dataBytes[4];
    memcpy(dataBytes, &data, 4);

    // send packet
    SendData(dataBytes, 4, type);
}

void DataClient::SendData(int data, DataType type)
{
    // convert to bytes
    uint8_t dataBytes[4];
    memcpy(dataBytes, &data, 4);

    // send packet
    SendData(dataBytes, 4, type);
}

void DataClient::SendData(const char *data, DataType type)
{
    // convert to bytes
    size_t length = strlen(data);

    uint8_t dataBytes[length];
    memcpy(dataBytes, data, length);

    // send packet
    SendData(dataBytes, length, type);
}
