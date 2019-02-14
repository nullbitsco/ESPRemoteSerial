#include "ESPRemoteSerial.h"

#ifdef USE_NTPCLIENT
ESPRemoteSerial::ESPRemoteSerial() : _timeClient(_ntpUDP) {}
#endif

ESPRemoteSerial::~ESPRemoteSerial(void) {
    if (_internal) {
        delete _server;
    }
}

// Public

#ifdef USE_NTPCLIENT
void ESPRemoteSerial::setTZOffset(int offset) { 
    _TZoffset = offset * 3600;
}
#endif

void ESPRemoteSerial::begin(void) {
    SPIFFS.begin();

    #ifdef CLEAR_ON_BOOT
    _removeLog();
    #endif

    #ifdef USE_NTPCLIENT
    _timeClient.begin();
    _timeClient.setTimeOffset(_TZoffset);
    while(!_timeClient.update()) {
        _timeClient.forceUpdate();
    }
    #endif

    _logger.getAppender().push_back(new RollingFileAppender(_FILENAME, LOG_MAX_COLS, LOG_MAX_ROWS, true));
    _logger.addFormatterToAll([this](Print &output, Appender::Level level, const char *msg, va_list *args) {

        // Print arch in log messages
        output.print("[");
        #if defined(ESP32)
        output.print("ESP32");
        #elif defined (ESP8266)
        output.print("ESP8266");
        #else
        output.print("REMOTE");
        #endif
        output.print("] ");

        #ifdef USE_NTPCLIENT
            _formattedDate = _timeClient.getFormattedDate();
            output.print(_formattedDate.c_str());
        #endif
        output.print(" | ");

        size_t length = vsnprintf(NULL, 0, msg, *args) + 1;
        char buffer[length];

        vsnprintf(buffer, length, msg, *args);
        output.print(buffer);
    });

    // Start server if internal
    if (_internal) {
        if (NULL == _server) {
            print("Starting internal AsyncWebServer");
            _server = new AsyncWebServer(80); 
        }
        _server->begin();
    }

    #ifdef SERVE_COMPRESSED
    _server->on("/log", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", log_html_gz, log_html_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    #else
    _server->serveStatic("/log", SPIFFS, "/log.html");
    #endif

    _server->on("/logs", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!SPIFFS.exists("/default.log")) {
            request->send(200, "text/html", "No log on device");
        } else if (request->header("If-Modified-Since").equals(String(_logLastModified, HEX))) {
            request->send(304);
        } else {
            AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/default.log", "text/html");
            response->addHeader("Last-Modified", String(_logLastModified, HEX));
            request->send(response);       
        }

    });

    _server->on("/logs", HTTP_DELETE, [this](AsyncWebServerRequest *request) {
        _clearLog();
        request->send(200);
    });

    _server->on("/reset", HTTP_ANY, [this](AsyncWebServerRequest *request) {
        request->send(200);
        Serial.println("Rebooting!");
        delay(100);
        ESP.restart();
    });

    _server->onNotFound([this](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "404: Not found. Are you at /log?");

    });

    print("ESPRemoteSerial initialized!");

    #ifdef SERVE_COMPRESSED
    print("Serving in compressed html mode.");
    #endif
}

void ESPRemoteSerial::bind(AsyncWebServer* server) {
    _internal = false;
    _server = server;
}

// Private
void ESPRemoteSerial::_clearLog(void) {
    if (!SPIFFS.exists("/default.log")) return;
    File file = SPIFFS.open("/default.log", "w");
    file.print("129"); //default offset
    file.println(std::string(125, ' ').c_str());
    file.close();
    _logLastModified = millis();
}

void ESPRemoteSerial::_removeLog(void) {
    if (!SPIFFS.exists("/default.log")) return;
    SPIFFS.remove("/default.log");
}

void ESPRemoteSerial::_notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "404: Not found. Are you at /log? Did you forget to upload log.html to the SPIFFS?");
}
