#ifndef REMOTE_SERIAL_H
#define REMOTE_SERIAL_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Log4Esp.h>
#include <WiFiUdp.h>
#include <FS.h>

#include "compressedHTML.h"

#define USE_NTPCLIENT       // Use NTP in log timestamp
#define CLEAR_ON_BOOT       // Clear the previous log on boot
#define SERVE_COMPRESSED    // Serve the embedded version of log.html
#define LOG_MAX_ROWS 30     // Maximum number of rows in log
#define LOG_MAX_COLS 128    // Maximum number of cols in log

#define _FILENAME "/default.log"

#ifdef USE_NTPCLIENT
    #include <NTPClient.h>
#endif

class ESPRemoteSerial {

public: 
    #ifdef USE_NTPCLIENT
    ESPRemoteSerial();
    #endif

    ~ESPRemoteSerial(void);

    #ifdef USE_NTPCLIENT
    void setTZOffset(int offset);
    #endif

    void begin(AsyncWebServer* server = NULL);
    template <class T, typename... Args> void print(T msg, Args... args) {
        _logger.trace(msg, args...);
        _logLastModified = millis();
    }

private:
    AsyncWebServer* _server;
    Logger _logger;
    String _formattedDate;
    String _dayStamp;
    String _timeStamp;

    unsigned long _logLastModified = 0;

    #ifdef USE_NTPCLIENT
    int _TZoffset = 0;
    WiFiUDP _ntpUDP;
    NTPClient _timeClient;
    #endif

    bool _internal = true;
    void _clearLog(void);
    void _removeLog(void);
};

#endif