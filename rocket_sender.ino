#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>

int a = 1;

const int soundPin = 6;
unsigned long soundtime = 0;
int flag_sound = 0;
int flag_s = 0;

const int descentSound[] = {1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0}; // C4 to C5
const int noteDuration = 100; // Duration of each note in milliseconds
int melodyIndex = 0;

#define RX_PIN 5  // Пин для приёма данных
#define LORA_SS 10
#define LORA_RST 9
#define LORA_DIO0 2

// Создаем объект SoftwareSerial. RX - пин 5, TX - пин 6 (TX не используется в данном случае)
SoftwareSerial mySerial(RX_PIN, 3); // RX, TX


String receivedMessage = ""; // Переменная для сборки сообщения


void setup() {
   pinMode(soundPin, OUTPUT);
  // Инициализация SoftwareSerial для приема данных
  mySerial.begin(9600);
  
  // Инициализация стандартного Serial для отладки
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");

  // Инициализация LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) { 
    Serial.println("Ошибка инициализации LoRa!");
    while (1);
  digitalWrite(soundPin, LOW);
  }
}

void loop() {
  // Проверяем, есть ли данные для чтения
  if (mySerial.available()){
    char c = mySerial.read(); // Читаем один символ
    if (c == '\n') {
      // Если символ новой строки (\n), сообщение завершено
      //Serial.print("Получено сообщение: ");
      //Serial.println(receivedMessage);

      // Отправляем сообщение через LoRa
      LoRa.beginPacket();
      LoRa.print(receivedMessage);
      LoRa.endPacket();
      //Serial.println("Сообщение отправлено через LoRa.");

      // Очищаем переменную для следующего сообщения
      receivedMessage = "";
    } else {
      // Добавляем символ к сообщению
      receivedMessage += c;
    }
  }
  int val1 = analogRead(A0);
  Serial.println(val1);
  if (val1 >= 800){
    ls_sygnal();
  }
}

void ls_sygnal() { // Звуковое оповещение посадки
  // включаем пьезодинамик
  if (millis() - soundtime >= noteDuration) {
    soundtime = millis();
    if (melodyIndex < sizeof(descentSound) / sizeof(descentSound[0])) {
      digitalWrite(soundPin, descentSound[melodyIndex]);
      melodyIndex++;
    } else {
      melodyIndex = 0; // Reset melody index to replay
    }
  }
}
