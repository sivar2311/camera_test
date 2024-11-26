#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProCamera.h>

#include "camera.h"

const char* WIFI_SSID = "";
const char* WIFI_PASS = "";

const char* DEVICE_ID  = "";
const char* APP_KEY    = "";
const char* APP_SECRET = "";

bool handleSnapshot(const String& deviceId) {
    camera_fb_t* fb = esp_camera_fb_get();

    if (!fb) {
        Serial.println("Failed to grab image");
        return false;
    }
    SinricProCamera& myCamera = SinricPro[deviceId];

    int result = myCamera.sendSnapshot(fb->buf, fb->len);

    esp_camera_fb_return(fb);

    return result = 200;
}

bool onPowerState(const String& deviceId, bool& state) {
    return true;
}

void setupWiFi() {
    Serial.print("Connecting Wifi");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void setupSinricPro() {
    SinricProCamera& myCamera = SinricPro[DEVICE_ID];
    myCamera.onPowerState(onPowerState);
    myCamera.onSnapshot(handleSnapshot);
    SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
    SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });

    SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupSinricPro();
    setupCamera();
}

void loop() {
    SinricPro.handle();
}
