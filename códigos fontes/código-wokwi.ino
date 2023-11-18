/////////--------IOT--------FIAP------------///////////

#include <WiFi.h>
#include <DHTesp.h>
#include <PubSubClient.h>

// Configurações de WiFi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";  // Substitua pelo sua senha

// Configurações de MQTT
const char *BROKER_MQTT = "46.17.108.113";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "fiware_Healthharbor001";
const char *TOPIC_PUBLISH_CARD = "/TEF/healthharbor001/attrs/c";


// Configurações de Hardware
#define PIN_POTE 12
#define PUBLISH_DELAY 2000

// Variáveis globais
WiFiClient espClient;
PubSubClient MQTT(espClient);
DHTesp dht;
unsigned long publishUpdate = 0;
TempAndHumidity sensorValues;
const int TAMANHO = 200;

// Protótipos de funções
void updateSensorValues();
void initWiFi();
void initMQTT();
void reconnectMQTT();
void reconnectWiFi();
void checkWiFIAndMQTT();

void updateSensorValues() {
  sensorValues = dht.getTempAndHumidity();
}

void initWiFi() {
  Serial.print("Conectando com a rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso: ");
  Serial.println(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar com o Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
     } else {
      Serial.println("Falha na conexão com MQTT. Tentando novamente em 2 segundos.");
      delay(2000);
    }
  }
}

void checkWiFIAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) reconnectWiFi();
  if (!MQTT.connected()) reconnectMQTT();
}

void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Wifi conectado com sucesso");
  Serial.print(SSID);
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  dht.setup(PIN_POTE, DHTesp::DHT22);
  initWiFi();
  initMQTT();
}

void loop() {
  checkWiFIAndMQTT();
  MQTT.loop();
  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();
    updateSensorValues();
        
    //temperatura
    char msgBuffer1[7];
    int value = analogRead(A0);  
    Serial.print("Batimentos cardíacos: ");
    Serial.println(value);
    dtostrf(value, 4, 2, msgBuffer1);
    MQTT.publish(TOPIC_PUBLISH_CARD,msgBuffer1);
    }
}

