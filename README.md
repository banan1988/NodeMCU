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
If your program need PIN's number from GPIO you can use defined const values like D1, D2 etc. (Library for EPSx has already defined map for it)
