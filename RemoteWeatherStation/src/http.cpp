#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

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
