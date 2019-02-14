# ESPRemoteSerial
Remote (network) serial monitor for ESP8266 &amp; ESP32.

## Features
- Self-hosted ESP network monitor that emulates the Arduino serial monitor
- Useful for debugging and development on remote ESP-based systems
- Automatically prints to local serial if initialized
- Well-suited for use with arduinoOTA update
- Works with mDNS for accessing via a local hostname rather than IP address

## Installation
1. Install the dependencies from the section below.
1. Add this library to the Arduino library, either by downloading it and adding it as a .zip, copying it locally into the libraries folder, or adding this repo address into the Arduino library manager list.
1. `#include <ESPRemoteSerial.h>` in your .ino sketch file.

## Dependencies
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [Log4Esp](https://github.com/hunsalz/log4Esp)
- [NTPClient](https://github.com/taranais/NTPClient) (Only if compiled with `USE_NTPCLIENT`, which is on by default) 

## Usage
See complete examples in the [examples](https://github.com/nullbitsco/ESPRemoteSerial/tree/master/examples) directory.
### Printing to remote log
Call `ESPRemoteSerial::print()` or `ESPRemoteSerial::println()`. Supports formatters in the same way `printf()` or `std::cout` does.
```C
remoteSerial.print("setup() complete | T = %u", millis());
```
### Viewing remote log
Visit `http://local_ip_or_host/log` (i.e. `192.168.1.101/log`) or the hostname if using mDNS (i.e. `wifithingy.local/log`). Since the logger is persistent, it will load messages logged before loading the remote log.

## To-do
1. Locally include Bootstrap and jQuery and inline them into log.html (for networks without internet access, since these files are currently fetched from their respective CDNs)
