# ESP32 Sensor Client 📟

Código embarcado para ESP32 que lê dados do sensor **MPU6050** (aceleração) e envia periodicamente para uma **API REST** hospedada no Azure.

---

## 📦 Funcionalidades

- Conexão automática ao Wi-Fi
- Sincronização de horário via NTP
- Leitura de aceleração em X, Y e Z com o MPU6050
- Envio periódico dos dados em JSON via HTTP POST
- Calibração manual do sensor com botão físico
- Comunicação com API via HTTPS

---

## ⚙️ Hardware Utilizado

- ESP32 (WROOM-32 ou equivalente)
- Sensor MPU6050 (I2C)
- Botão conectado ao pino 33 (GND quando pressionado)

---

## 🔌 Pinos I2C configurados

| Função      | Pino ESP32 |
|-------------|------------|
| SDA         | GPIO 27    |
| SCL         | GPIO 26    |
| Botão       | GPIO 33    |

---

## 📡 Requisições para API

O ESP32 envia os dados no formato:

```json
{
  "deviceId": "f68fb3dd-65b9-47c3-b638-1bad36be5e37",
  "accelX": -0.12,
  "accelY": 0.05,
  "accelZ": 9.81,
  "temperature": 0,
  "timestamp": "2025-05-06T22:00:00Z"
}
```
## 🌐 Configurações Wi-Fi e API
Altere no código conforme sua rede e endpoint:
```bash
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";
const char* server = "https://SEU_ENDPOINT/api/SensorReadings";
const char* deviceId = "GUID_DO_DISPOSITIVO";
```
## 🔁 Calibração do Sensor
Mantenha o botão pressionado para recalibrar o sensor (aceleração e giroscópio). Isso será mostrado via Serial.

## 🧪 Depuração
O código envia mensagens no Serial Monitor (baud 115200):

- Status da conexão Wi-Fi
- Horário sincronizado
- JSON gerado
- Código de resposta HTTP (ex: 200, 400)

## 📚 Bibliotecas necessárias
Instale no Arduino IDE:

- WiFi (incluso com ESP32)
- HTTPClient (incluso com ESP32)
- MPU6050_light (por rfetick)
- ArduinoJson (por Benoit Blanchon)

