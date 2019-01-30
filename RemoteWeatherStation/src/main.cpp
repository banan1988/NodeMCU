#include <Arduino.h>

#include <DHT.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <ESP8266WiFi.h>

#include <InfluxDb.h>

#include <Private.h>

#ifdef ESP32
#pragma message(THIS PROGRAM IS FOR ESP8266 ONLY !)
#error Select ESP8266 board.
#endif

#define INFLUXDB_HOST "192.168.0.100"
#define INFLUXDB_DATABASE "RemoteWeatherStation"

#define HOSTNAME "NodeMCU v2"

#define ONEWIRE_PIN D1
OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);

#define DHT_PIN D2
#define DHT_TYPE DHT::DHT11
DHT dht;

Influxdb influx(INFLUXDB_HOST);

void elapsedTime(long time)
{
    Serial.print("---- ");
    Serial.print(millis() - time);
    Serial.println("ms");
}

void wifiConnect()
{
    unsigned long time = millis();

    Serial.print("Connecting to ");
    Serial.println(WIFI_ssid);

    wifi_station_set_hostname(HOSTNAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_ssid, WIFI_password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // Enable light sleep
    // wifi_set_sleep_type(LIGHT_SLEEP_T);

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());

    elapsedTime(time);
}

void initDTH11()
{
    unsigned long time = millis();

    // init DHT11 sensor
    Serial.println("init DHT11 sensor");
    dht.setup(DHT_PIN, DHT_TYPE);

    elapsedTime(time);
}

void initDallasSensors()
{
    unsigned long time = millis();

    Serial.println("init Dallas(ds18b20) sensors");
    sensors.begin();

    elapsedTime(time);
}

InfluxData readTemperatureDs18b20()
{
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);

    Serial.println("readTemperatureDs18b20: " + String(temperature));

    // create a measurement object
    InfluxData measurement("temperature");
    measurement.addTag("device", ARDUINO_BOARD);
    measurement.addTag("sensor", "ds18b20");
    measurement.addValue("value", temperature);

    return measurement;
}

InfluxData readTemperatureDHT11()
{
    float temperature = dht.getTemperature();

    Serial.println("readTemperatureDHT11: " + String(temperature));

    // create a measurement object
    InfluxData measurement("temperature");
    measurement.addTag("device", ARDUINO_BOARD);
    measurement.addTag("sensor", "dht11");
    measurement.addValue("value", temperature);

    return measurement;
}

InfluxData readHumidityDHT11()
{
    float humidity = dht.getHumidity();

    Serial.println("readHumidityDHT11: " + String(humidity));

    // create a measurement object
    InfluxData measurement("humidity");
    measurement.addTag("device", ARDUINO_BOARD);
    measurement.addTag("sensor", "dht11");
    measurement.addValue("value", humidity);

    return measurement;
}

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(2000);

    // WiFi
    wifiConnect();

    // InfluxDB
    influx.setDb(INFLUXDB_DATABASE);

    // Sensors
    initDTH11();
    initDallasSensors();
}

void loop()
{
    influx.prepare(readTemperatureDs18b20());

    influx.prepare(readTemperatureDHT11());
    influx.prepare(readHumidityDHT11());

    influx.write();

    Serial.println("I'm going into deep sleep mode for 60 seconds");
    // delay(6000);
    ESP.deepSleep(60e6); 
}
