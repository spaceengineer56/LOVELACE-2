// Valid Version 2.9
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include "MPU6050.h"

MPU6050 mpu;
Adafruit_BMP280 bmp;
TinyGPSPlus gps;
SoftwareSerial ss(8, 5); // GPS Module

struct TData {
    float temp; // Температура
    float att;  // Высота
    float flat; // Широта
    float flon; // Долгота
};

TData Data;
uint32_t sendtime;
uint32_t mortime;
bool flagmort = false;
float max_alt = -9999;

void setup() {
    Wire.begin();
    pinMode(9, OUTPUT); // Пьезо
    pinMode(10, OUTPUT); // Мортира 1
    pinMode(3, OUTPUT); // Мортира 2
    pinMode(4, OUTPUT); // Картридер
    ss.begin(9600);
    bmp.begin();
    mpu.initialize();
    SD.begin(4);
}

void loop() {
    float altitude_total = bmp.readAltitude(1013.25);
    max_alt = max(altitude_total, max_alt);

    if ((max_alt - altitude_total > 25) and (max_alt >= 700)) {
        descent();
    }

    while (ss.available()) {
        gps.encode(ss.read());
    }

    if (gps.location.isUpdated()) {
        Data.flat = gps.location.lat();
        Data.flon = gps.location.lng();
    }

    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float accelY = ay * (9.81 / 16384.0);
    float gyroX = gx / 131.0;
    float gyroY = gy / 131.0;
    float gyroZ = gz / 131.0;

    Data.temp = bmp.readTemperature();
    Data.att = altitude_total;
    File dataFile = SD.open("data.txt", FILE_WRITE);
    if (dataFile) {
        dataFile.print(millis()); dataFile.print(";");
        dataFile.print(Data.temp); dataFile.print(";");
        dataFile.print(Data.flat, 8); dataFile.print(";");
        dataFile.print(Data.flon, 8); dataFile.print(";");
        dataFile.print(Data.att); dataFile.print(";");
        dataFile.print(accelY); dataFile.print(";");
        dataFile.print(gyroX); dataFile.print(";");
        dataFile.print(gyroY); dataFile.print(";");
        dataFile.print(gyroZ);
        dataFile.println();
        dataFile.close();
    }

    // Отправка данных через SoftwareSerial
    if (millis() - sendtime >= 500) {
        sendtime = millis();
        send_data(Data.flat, Data.flon, Data.att);
    }
}


void send_data(float flat, float flon, float att) {
    // Формируем строку для отправки
    String message = String(flat, 8) + ";" + String(flon, 8) + ";" + String(att, 2);
    ss.println(message); // Отправляем данные через SoftwareSerial
    //Serial.println(message);
}

void sound() {
      digitalWrite(9, HIGH);
}

void descent() {
    digitalWrite(10, HIGH); // Активация системы раскрытия парашюта
    if (!flagmort) {
        mortime = millis();
        flagmort = true;
    }

    if (flagmort && (millis() - mortime >= 1500)) {
        digitalWrite(3, HIGH); 
    }
    sound();
}
