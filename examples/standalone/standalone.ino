/*
Standalone example for ESPRemoteSerial library.
This example illustrates use in an application where no webserver is already present.
The ESPRemoteSerial library will automatically spin up an ESPAsyncWebServer.
Go to http://server_ip_or_hostname/log to see the remote serial monitor.
*/

#include <ESPRemoteSerial.h>

// For initial wifi setup
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

// Change these for your SSID/PASS
#define WIFI_SSID "somessid"
#define WIFI_PASS "somepass"

#define LED_PIN 2

ESPRemoteSerial remoteSerial;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

    // Call ESPRemoteSerial::begin() after wifi connection has been set up.
    remoteSerial.begin();

    remoteSerial.print("setup() complete | T = %u", millis());
}

void loop() {
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
}