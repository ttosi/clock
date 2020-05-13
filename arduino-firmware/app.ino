#include <Wire.h>
#include <math.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN 7
#define PIXEL_NUM 4

Adafruit_7segment Matrix = Adafruit_7segment();
Adafruit_NeoPixel pixels(PIXEL_NUM, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint8_t prevTime;
uint8_t hour;
uint8_t minute;
uint8_t second;
bool hasColon = true;

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    Wire.begin(0x13);
    Wire.onReceive(receive);

    Matrix.begin(0x70);
    Matrix.setBrightness(8);

    pixels.begin();
    pixels.fill(pixels.Color(250, 69, 0), 0, PIXEL_NUM); // orange
    // pixels.fill(pixels.Color(255, 0, 0), 0, PIXEL_NUM);    // red
    // pixels.fill(pixels.Color(0, 0, 255), 0, PIXEL_NUM);  // blue
    pixels.show();

    Serial.println("Arduino Ready!");
}

void loop()
{
    if (prevTime != hour + minute + second)
    {
        displayTime();
        prevTime = hour + minute + second;
    }

    delay(50);
}

void receive(int byteCount)
{
    if (Wire.available())
    {
        uint8_t index = 0;
        uint8_t time[byteCount];

        while (Wire.available())
            time[index++] = Wire.read();

        hour = time[0];
        minute = time[1];
        second = time[2];
    }
}

void displayTime()
{
    if (hour > 12) hour = hour - 12;
    if (hour / 10 % 10 == 0)
    {
        Matrix.writeDigitNum(0, ' ');
    }
    else
    {
        Matrix.writeDigitNum(0, 1);
    }

    Matrix.writeDigitNum(1, hour % 10);
    Matrix.drawColon(hasColon);
    Matrix.writeDigitNum(3, minute / 10 % 10);
    Matrix.writeDigitNum(4, minute % 10);
    Matrix.writeDisplay();

    hasColon = !hasColon;
}

void countDown(uint8_t duration)
{
    Matrix.drawColon(false);
    for (uint8_t s = duration; s > 0; s--)
    {
        Serial.println(s);
        Matrix.writeDigitNum(0, s / 10 % 10);
        Matrix.writeDigitNum(1, s % 10, true);

        for (uint8_t m = 99; m > 0; m--)
        {
            Matrix.writeDigitNum(3, m / 10 % 10);
            Matrix.writeDigitNum(4, m % 10);
            Matrix.writeDisplay();

            delay(10);
        }

        Matrix.writeDisplay();
    }
}