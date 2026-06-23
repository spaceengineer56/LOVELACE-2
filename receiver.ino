#include <SPI.h>
#include <LoRa.h>

// Настройки LoRa
#define LORA_SS 10
#define LORA_RST 9
#define LORA_DIO0 2

void setup() {
  // Инициализация стандартного Serial для связи с компьютером
  Serial.begin(115200);
  pinMode(8, OUTPUT);
  while (!Serial) {
    ; // Ждем подключения Serial (только для Arduino с USB)
  }

  // Инициализация LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) { 
    while (1); // Остановить выполнение, если LoRa не инициализировалась
  }
}

void loop() {
  // Проверяем, есть ли входящий пакет
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Получаем данные из пакета
    digitalWrite(8, 1);
    String receivedData = ""; // Переменная для хранения данных
    while (LoRa.available()) {
      receivedData += (char)LoRa.read(); // Читаем данные как строку
    }

    // Получаем RSSI (уровень сигнала)

    // Убираем лишние символы и форматируем строку
    receivedData.trim(); // Удаляем лишние пробелы и символы новой строки
    String formattedData = receivedData;

    // Приведение строки к длине 60 байтов
    formattedData = formatToFixedLength(formattedData, 60);
    
    // Отправляем данные в COM-порт
    Serial.println(formattedData);
    delay(5.374269);
    digitalWrite(8, 0);
  }
}

// Функция для приведения строки к фиксированной длине
String formatToFixedLength(String input, int length) {
  if (input.length() > length) {
    // Если строка длиннее, обрезаем её
    return input.substring(0, length);
  } else {
    // Если строка короче, дополняем пробелами
    while (input.length() < length) {
      input += ";"; // Добавляем пробелы
    }
    return input;
  }
}
