/*
Bound example for ESPRemoteSerial library.
This example illustrates use in an application where a webserver is already present.
Go to http://server_ip_or_hostname/log to see the remote serial monitor.
*/

#include <ESPRemoteSerial.h>

// For initial wifi/server setup
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

AsyncWebServer server(80);
DNSServer dns;

#define LED_PIN 2

ESPRemoteSerial remoteSerial;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);

    AsyncWiFiManager wifiManager(&server,&dns);
    wifiManager.autoConnect("AutoConnectAP");

    server.begin(); // Start the webserver like you normally would.

    remoteSerial.bind(&server); // Bind the server after calling server.begin() 
    remoteSerial.begin();

    remoteSerial.print("setup() complete | T = %u", millis());
}

void loop() {
    remoteSerial.print("loop() | T = %u", millis());
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
}