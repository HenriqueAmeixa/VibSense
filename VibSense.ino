#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <ArduinoJson.h>
#include <vector>

const char* ssid = "BACONLOGA";
const char* password = "a1b2c3d4e5";
const char* server = "https://apisensor.azurewebsites.net/api/SensorReadings/lote";
const char* deviceId = "f68fb3dd-65b9-47c3-b638-1bad36be5e37";
const char* apiKey = "abc123secretapikeyxyz";  // SUA API KEY

MPU6050 mpu(Wire);
const int pinBotaoCalibrar = 32;
const int pinBotaoEnvio = 33;

bool envioAtivo = true;
bool estadoAnteriorBotaoEnvio = HIGH;

struct LeituraMPU {
  float x, y, z;
  unsigned long timestamp;
};

const int taxaAmostragemHz = 140;
const unsigned long duracaoCaptura = 1000;
std::vector<LeituraMPU> bufferLeituras;

void setup() {
  Serial.begin(115200);
  Wire.begin(27, 26);

  pinMode(pinBotaoCalibrar, INPUT_PULLUP);
  pinMode(pinBotaoEnvio, INPUT_PULLUP);

  Serial.print("Conectando no WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

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

void capturarAmostrasPor1Segundo() {
  bufferLeituras.clear();
  unsigned long tempoInicio = millis();
  unsigned long intervaloAmostra = 1000 / taxaAmostragemHz;
  unsigned long proximaAmostra = tempoInicio;

  while (millis() - tempoInicio < duracaoCaptura) {
    if (millis() >= proximaAmostra) {
      mpu.update();
      LeituraMPU leitura;
      leitura.x = mpu.getAccX();
      leitura.y = mpu.getAccY();
      leitura.z = mpu.getAccZ();
      leitura.timestamp = millis();
      bufferLeituras.push_back(leitura);
      proximaAmostra += intervaloAmostra;
    }
    delay(1);
  }

  Serial.print("Capturadas ");
  Serial.print(bufferLeituras.size());
  Serial.println(" amostras.");
}

void enviarAmostras() {
  StaticJsonDocument<8192> doc;
  doc["deviceId"] = deviceId;
  doc["timestamp"] = getISO8601Time();
  JsonArray leituras = doc.createNestedArray("leituras");

  for (auto& l : bufferLeituras) {
    JsonObject obj = leituras.createNestedObject();
    obj["accelX"] = l.x;
    obj["accelY"] = l.y;
    obj["accelZ"] = l.z;
    obj["ms"] = l.timestamp;
  }

  String json;
  serializeJson(doc, json);
  Serial.print("Enviando lote com ");
  Serial.print(bufferLeituras.size());
  Serial.println(" amostras:");
  Serial.println(json);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("ApiKey ") + apiKey);  // ✅ header correto
    int response = http.POST(json);
    Serial.print("Código HTTP: ");
    Serial.println(response);
    http.end();
  } else {
    Serial.println("WiFi desconectado. Dados não enviados.");
  }
}

void loop() {
  mpu.update();

  if (digitalRead(pinBotaoCalibrar) == LOW) {
    Serial.println("Recalibrando MPU...");
    mpu.calcOffsets(true, true);
    Serial.println("Recalibração concluída.");
    delay(1000);
  }

  bool estadoAtualBotaoEnvio = digitalRead(pinBotaoEnvio);
  if (estadoAtualBotaoEnvio == LOW && estadoAnteriorBotaoEnvio == HIGH) {
    envioAtivo = !envioAtivo;
    Serial.print("Envio de dados: ");
    Serial.println(envioAtivo ? "ATIVADO" : "DESATIVADO");
    delay(500);
  }
  estadoAnteriorBotaoEnvio = estadoAtualBotaoEnvio;

  if (!envioAtivo) {
    delay(100);
    return;
  }

  capturarAmostrasPor1Segundo();
  enviarAmostras();
}
