#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <ArduinoJson.h>

const char* ssid = "BACONLOGA";
const char* password = "a1b2c3d4e5";
const char* server = "https://apisensor.azurewebsites.net/api/SensorReadings";
const char* deviceId = "f68fb3dd-65b9-47c3-b638-1bad36be5e37";

MPU6050 mpu(Wire);
const int pinBotao = 33;

void setup() {
  Serial.begin(115200);
  Wire.begin(27, 26);
  pinMode(pinBotao, INPUT_PULLUP);

  Serial.print("Conectando no WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  // Configura fuso horário UTC (offset 0) e servidores NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Sincronizando horário com NTP...");

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Erro ao obter horário NTP.");
  } else {
    Serial.println("Horário sincronizado.");
  }

  Serial.println("Inicializando MPU...");
  if (mpu.begin() != 0) {
    Serial.println("Erro ao iniciar MPU!");
  }
  mpu.calcOffsets(true, true);
  Serial.println("MPU pronto!");
}

String getISO8601Time() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "";
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buf);
}

void loop() {
  mpu.update();

  if (digitalRead(pinBotao) == LOW) {
    Serial.println("Recalibrando MPU...");
    mpu.calcOffsets(true, true);
    Serial.println("Recalibração concluída.");
    delay(1000);
  }

  StaticJsonDocument<256> doc;
  doc["deviceId"] = deviceId;
  doc["accelX"] = mpu.getAccX();
  doc["accelY"] = mpu.getAccY();
  doc["accelZ"] = mpu.getAccZ();
  doc["temperature"] = 0;  // Pode remover se não quiser enviar nada
  doc["timestamp"] = getISO8601Time();

  String json;
  serializeJson(doc, json);

  Serial.println("Enviando JSON:");
  Serial.println(json);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server);
    http.addHeader("Content-Type", "application/json");
    int response = http.POST(json);
    Serial.print("Código de resposta HTTP: ");
    Serial.println(response);
    http.end();
  } else {
    Serial.println("WiFi desconectado! Dados não enviados.");
  }

  delay(100); // 10 Hz
}
