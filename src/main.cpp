#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProCamera.h>

#include "camera.h"

const char* WIFI_SSID = "SinricWiFi";
const char* WIFI_PASS = "SinricWiFi";

const char* DEVICE_ID  = "6741903cdb15b093899448af";
const char* APP_KEY    = "ebf0cf46-97a2-4c4e-8cf3-e630e9b640ca";
const char* APP_SECRET = "5690fc71-938c-4174-a1de-270c2c3cffef-ffc31556-4219-4043-bb59-d2f6ac372d21";

bool handleSnapshot(const String& deviceId) {
    camera_fb_t* fb = esp_camera_fb_get();

    if (!fb) {
        Serial.println("Failed to grab image");
        return false;
    }
    SinricProCamera& myCamera = SinricPro[deviceId];

    int result = myCamera.uploadImage(fb->buf, fb->len);

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