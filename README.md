# ESP32 Sensor Client 📟

Código embarcado para ESP32 que lê dados do sensor **MPU6050** (aceleração) e envia periodicamente para uma **API REST** hospedada no Azure.

---

## 📦 Funcionalidades

- Conexão automática ao Wi-Fi
- Sincronização de horário via NTP
- Leitura de aceleração em X, Y e Z com o MPU6050
- Amostragem com taxa configurável (ex: 140 Hz)
- Envio em lote de leituras a cada 1 segundo
- Autenticação com API Key via header HTTP
- Dois botões físicos: um para ativar/desativar envio e outro para calibração
- Indicação do estado de envio via LED onboard (GPIO 2)
- Comunicação com API via HTTPS

---

## ⚙️ Hardware Utilizado

- ESP32 (WROOM-32 ou equivalente)
- Sensor MPU6050 (I2C)
- Botão 1: ativar/desativar envio (GPIO 33)
- Botão 2: recalibrar sensor (GPIO 32)

---

## 🔌 Pinos Utilizados

| Função              | Pino ESP32 |
|---------------------|------------|
| SDA (I2C)           | GPIO 27    |
| SCL (I2C)           | GPIO 26    |
| Botão de envio      | GPIO 33    |
| Botão de calibração | GPIO 32    |
| LED Status          | GPIO 2     |

---

## 📡 Requisição para API

O ESP32 envia os dados no formato de lote:

```json
{
  "deviceId": "f68fb3dd-65b9-47c3-b638-1bad36be5e37",
  "timestamp": "2025-05-07T19:00:00Z",
  "readings": [
    {
      "accelX": -0.004,
      "accelY": 0.002,
      "accelZ": 1.009,
      "millisecondsSinceStart": 123
    }
  ]
}
```

A chave de autenticação é enviada via header:

```
Authorization: ApiKey SUA_CHAVE_AQUI
```

---

## 🌐 Configurações Wi-Fi, API e Autenticação

No início do código, ajuste as seguintes constantes:

```cpp
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";
const char* server = "https://SEU_ENDPOINT/api/SensorReadings/lote";
const char* deviceId = "GUID_DO_DISPOSITIVO";
const char* apiKey = "SUA_API_KEY";
```

---

## 🔘 Controles por Botões

O projeto possui dois botões:

- **Botão de envio (GPIO 33):**
  - Pressionar uma vez: ativa ou desativa o envio de dados para a API.
  - O estado é indicado pelo LED onboard:
    - **Aceso** = envio ativado
    - **Apagado** = envio desativado

- **Botão de calibração (GPIO 32):**
  - Pressionar e manter: recalibra o sensor MPU6050 (aceleração e giroscópio).
  - A calibração é exibida no Serial Monitor.

---

## 💡 LED Indicador

- O LED onboard (GPIO 2) indica se o envio está ativo:
  - **Aceso** = envio de dados ativado
  - **Apagado** = envio desativado

---

## 🧪 Depuração

O código envia mensagens via Serial Monitor (baud 115200):

- Status da conexão Wi-Fi
- Horário sincronizado
- JSON gerado para envio
- Código de resposta HTTP (ex: 200, 400, 403)

---

## 📚 Bibliotecas Necessárias

Instale no Arduino IDE:

- `WiFi` (incluso com ESP32)
- `HTTPClient` (incluso com ESP32)
- `MPU6050_light` (por rfetick)
- `ArduinoJson` (por Benoit Blanchon)
