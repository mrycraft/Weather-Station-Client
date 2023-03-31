#include "Camera.h"
#include "HardwareSerial.h"

uint8_t* Camera::TakeImage(size_t* bufferLength)
{
    uint8_t* buffer = NULL;

    frameBuffer = esp_camera_fb_get();
    if (frameBuffer == NULL)
    {
        Serial.println("Camera capture failed");
    }
    else
    {
        if (frameBuffer->format != PIXFORMAT_JPEG)
        {
            bool convertSuccess = frame2jpg(frameBuffer, 70, &buffer, bufferLength);
            if (!convertSuccess) Serial.println("JPEG compression failed!");
        }
        else
        {
            *bufferLength = frameBuffer->len;
            buffer = frameBuffer->buf;
        }
    }

    esp_camera_fb_return(frameBuffer);
    frameBuffer = NULL;

    return buffer;
}

void Camera::Init()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (psramFound())
    {
        Serial.println("PSRAM found, initializing camera with 1600x1200 resolution");
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    }
    else
    {
        Serial.println("No PSRAM found, initializing camera with 800x600 resolution");
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t *s = esp_camera_sensor_get();
    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_VGA);
}
