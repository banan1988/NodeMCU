# NodeMCU

## Drivers for Windows
https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip

## Arduino IDE
https://www.arduino.cc/en/main/software

### How to start with ESPx in Arduino IDE
1. Open Arduino IDE
2. Open settings (Ctrl + ,)
3. Put http://arduino.esp8266.com/stable/package_esp8266com_index.json to "Additional Boards Manager URLs"
4. Select from menu: Tools/Board/Boards Manager
5. Search by "ESP" and install "esp8266 by EPS8266 Community"
6. Restart IDE
7. Select from menu: Tools/Board/NodeMCU 1.0 (ESP-12E Module)

### Libraries
Select from menu: Tools/Manage Libraries... (Ctrl + Shift + I)

### OneWire
Search and install OneWire library

### Dallas (ds18b20)
Search and install DallasTemperature library

### DHT11
Search DHT11 and install DHT sensor library for ESPx

### Examples
Select from menu: File/Examples

## Before you start play with ESPx
https://raw.githubusercontent.com/nodemcu/nodemcu-devkit-v1.0/master/Documents/NODEMCU_DEVKIT_V1.0_PINMAP.png
If your program need PIN's number from GPIO you can use defined const values like D1, D2 etc. (Library for EPSx has already defined map for it).

## How to test printing from your program via Putty ?
https://github.com/banan1988/NodeMCU/blob/master/putty.png

## PlatformIO IDE for vscode
https://platformio.org/install/ide?install=vscode
https://docs.platformio.org/en/latest/ide/vscode.html#quick-start

https://github.com/esp8266/Arduino/tree/master/doc/esp8266wifi
http://esp8266.github.io/Arduino/versions/2.0.0-rc2/doc/libraries.html
https://majsterkowo.pl/tani-bezprzewodowy-programator-avr-esp8266-wifi/
https://www.cron.dk/esp8266-on-batteries-for-years-part-1/