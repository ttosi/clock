#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <time.h>

#define STATUS_PIN 13

const char *ssid = "CenturyLink8092";
const char *password = "cx3877cf7k8vc6";

void setup()
{
    Serial.begin(115200);

    pinMode(13, OUTPUT);

    Serial.print("\r\n\r\nConnecting to: ");
    Serial.print(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\r\n");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Wire.begin();

    Serial.println("ESP Ready!");
}

void loop()
{
    uint8_t *time = getTime();
    transmitTime(time);
    delay(995);
}

uint8_t *getTime()
{
    static uint8_t time[3];

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin("http://worldtimeapi.org/api/timezone/America/Phoenix");
        int httpCode = http.GET();

        if (httpCode > 0)
        {
            String payload = http.getString();

            const size_t capacity = JSON_OBJECT_SIZE(15) + 300;
            DynamicJsonDocument doc(capacity);

            deserializeJson(doc, payload);
            const char *datetime = doc["datetime"];

            struct tm tm;
            time_t t;

            strptime(datetime, "%Y-%m-%dT%H:%M:%S", &tm);
            time[0] = tm.tm_hour;
            time[1] = tm.tm_min;
            time[2] = tm.tm_sec;
        }

        http.end();
    }

    return time;
}

void transmitTime(uint8_t *time)
{
    Wire.beginTransmission(0x13);
    Wire.write(time, 3);
    Wire.endTransmission();
}
