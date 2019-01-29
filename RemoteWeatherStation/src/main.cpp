#include <Arduino.h>

#include <DHTesp.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <InfluxDb.h>

#include <Private.h>

#ifdef ESP32
#pragma message(THIS PROGRAM IS FOR ESP8266 ONLY !)
#error Select ESP8266 board.
#endif

#define INFLUXDB_HOST "192.168.0.100"
#define INFLUXDB_DATABASE "RemoteWeatherStation"

#define HOSTNAME "NodeMCU v2"

OneWire oneWire(D1);
DallasTemperature sensors(&oneWire);
DHTesp dht;

Influxdb influx(INFLUXDB_HOST);

void elapsedTime(long time)
{
    Serial.print(millis() - time);
    Serial.println("ms");
}

void initDTH11()
{
    unsigned long time = millis();

    // init DHT11 sensor
    dht.setup(D2, DHTesp::DHT11);

    elapsedTime(time);
}

void initDallasSensors()
{
    unsigned long time = millis();

    // init Dallas(ds18b20) sensors
    sensors.begin();

    elapsedTime(time);
}

void wifiConnect()
{
    unsigned long time = millis();

    Serial.print("Connecting to ");
    Serial.println(WIFI_ssid);

    wifi_station_set_hostname("NodeMCU v2 #1");
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
    Serial.println("");

    elapsedTime(time);
}

void sendHttpPost(String temperature_ds18b20, String temperature, String humidity)
{
    if (WiFi.status() == WL_CONNECTED)
    {                    //Check WiFi connection status
        HTTPClient http; //Declare object of class HTTPClient

        String URL = "http://192.168.0.104:5000/weather/samples";

        Serial.println("Try send request to " + URL); //Print HTTP return code

        http.begin(URL);                                    //Specify request destination
        http.addHeader("Content-Type", "application/json"); //Specify content-type header

        String json = "{    \"sensors\": {        \"ds18b20\": {            \"temperature\": {                \"value\": \"" + temperature_ds18b20 + "\",                \"unit\": \"C\"            }        },        \"dht11\": {            \"temperature\": {                \"value\": \"" + temperature + "\",                \"unit\": \"C\"            },            \"humidity\": {                \"value\": \"" + humidity + "\",                \"unit\": \"%\"            }        }    }}";
        int httpCode = http.POST(json);    //Send the request
        String payload = http.getString(); //Get the response payload

        Serial.println(httpCode); //Print HTTP return code
        Serial.println(payload);  //Print request response payload

        http.end(); //Close connection
    }
    else
    {
        Serial.println("Error in WiFi connection");
    }
}

InfluxData readTemperatureDs18b20()
{
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);

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

    // create a measurement object
    InfluxData measurement("humidity");
    measurement.addTag("device", ARDUINO_BOARD);
    measurement.addTag("sensor", "dht11");
    measurement.addValue("value", humidity);

    return measurement;
}

void rawSerial()
{
    // sensors.requestTemperatures();
    // float temperature_ds18b20 = sensors.getTempCByIndex(0);

    // float humidity = dht.getHumidity();
    // float temperature = dht.getTemperature();

    // Serial.print(dht.getStatusString());
    // Serial.print("\t\t");
    // Serial.print(humidity, 1);
    // Serial.print("\t\t");
    // Serial.print(temperature, 1);
    // Serial.print("\t\t");
    // Serial.print(temperature_ds18b20, 1);
    // Serial.print("\t\t");
    // Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
    // Serial.println();
}

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(2000);

    Serial.println();
    Serial.println(ARDUINO_BOARD);
    Serial.println();

    // WiFi
    wifiConnect();

    // InfluxDB
    influx.setDb(INFLUXDB_DATABASE);

    // Sensors
    initDTH11();
    initDallasSensors();

    // Serial.println("DHT status\tHumidity (%)\tTemperature (C)\t ds18b20 (C)\tHeatIndex (C)");
    // Serial.println();
}

void loop()
{
    delay(5000);

    influx.prepare(readTemperatureDs18b20());
    influx.prepare(readTemperatureDHT11());
    influx.prepare(readHumidityDHT11());

    // writes all prepared measurements with a single request into db.
    boolean success = influx.write();

    // sendHttpPost(String(temperature_ds18b20), String(temperature), String(humidity));

    Serial.println("I'm going into light sleep mode for 5 seconds");
}
