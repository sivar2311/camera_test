#include <Arduino.h>
#include <HTTPClient.h>
#include <SinricPro.h>
#include <SinricProCamera.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "camera.h"

const char* WIFI_SSID = "";
const char* WIFI_PASS = "";

const char* DEVICE_ID  = "";
const char* APP_KEY    = "";
const char* APP_SECRET = "";

bool shouldUploadImage = false;

int uploadImage(camera_fb_t* fb, const String& server, const String& deviceId, unsigned long timestamp) {
    Serial.println("Uploading... ");

    if (!fb) return -1;

    WiFiClientSecure client;
    client.setInsecure();

    String signature = "blahblah";

    Serial.print("Signature:");
    Serial.println(signature);

    HTTPClient http;
    if (!http.begin(client, server, 443, "/snapshot", true)) return -1;
    http.addHeader("deviceid", deviceId);
    http.addHeader("timestamp", String(timestamp));
    http.addHeader("signature", signature);
    int resCode = http.POST(fb->buf, fb->len);
    http.end();

    Serial.print("Response Code:");
    Serial.println(resCode);

    return resCode;
}


bool onPowerState(const String& deviceId, bool& state) {
    if (!state) return true;

    shouldUploadImage = true;

    state = false;
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

    SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
    SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });

    SinricPro.begin(APP_KEY, APP_SECRET, "testws.sinric.pro");
}

void handleImageUpload() {
    if (!shouldUploadImage) return;

    camera_fb_t* fb = esp_camera_fb_get();

    if (!fb) {
        Serial.println("Failed to grab image");
        return;
    }

    int res = uploadImage(fb, "testcamera.sinric.pro", DEVICE_ID, SinricPro.getTimestamp());
    esp_camera_fb_return(fb);
    Serial.println(res);

    shouldUploadImage = false;
}

void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupSinricPro();
    setupCamera();
}

void loop() {
    SinricPro.handle();
    handleImageUpload();
}