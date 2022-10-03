#include <WiFi.h> // Biblioteca que permite com que o ESP32 se conecte a rede Wifi.
#include <DHT.h> // Biblioteca que permite usar o SENSOR DHT11.
#include <WiFiClient.h>
#include <PubSubClient.h>
#define DHTTYPE DHT11
#define DHTPIN 23 //defina o pino digital ao qual o pino de dados do sensor DHT está conectado,
// foi usado o pino GPIO4 neste exemplo.

WiFiClient ESPWiFiClient;
PubSubClient mqttClient(ESPWiFiClient);

const char* wifi_ssid = "IMD0902"; // Nome da rede Wifi.
const char* wifi_password = "imd0902iot"; // Senha da rede Wifi.
int wifi_timeout = 2000000;

const char* mqtt_broker = "broker.hivemq.com";
const int mqtt_port = 1883; // porta do mqtt_broker
int mqtt_timeout = 10000;

int led = 2; // So pra testar o funcionamento no ESP32, acender o LED 2.

//Criado um objeto DHT chamado sensor no pino e com o tipo
// de sensor que foi especificado anteriormente.

DHT sensor(DHTPIN, DHTTYPE);
int ruidoPin = 36;
int ruido = 0;


void ConectarWifi(){
    WiFi.mode(WIFI_STA); // "station mode": permite o ESP32 ser um cliente da rede Wifi.
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.print("Conectando a rede Wifi ... ");

    unsigned long startTime = millis();

    while(WiFi.status() != WL_CONNECTED && (millis() - startTime < wifi_timeout)){
        Serial.print(".");
        delay(100);
    }

    Serial.println();

    if(WiFi.status() != WL_CONNECTED){
        Serial.println("Falhou!");
    }else{
        Serial.print("Conectado com o IP: ");
        Serial.println(WiFi.localIP());
    }
}


void ConectarMQTT(){
    Serial.print("Reconectando ao MQTT Broker ...");

    unsigned long startTime = millis();
    while(!mqttClient.connected() && (millis() - startTime < mqtt_timeout)){
        Serial.print(".");
        String clientId = "ESP32ClientTeste-";
        clientId += String(random(0xffff), HEX);

        if(mqttClient.connect(clientId.c_str())){
            Serial.println();
            Serial.print("Conectado ao broker MQTT!");
        }
        delay(100);
    }
    Serial.println();
}


void setup(){
    Serial.begin(115200);
    pinMode(led, OUTPUT);
    ConectarWifi();

    if(WiFi.status() == WL_CONNECTED){
        Serial.println("Conectando ao broker MQTT ..");
        mqttClient.setServer(mqtt_broker, mqtt_port);
    }

    sensor.begin();
}

void loop(){
    delay(1000);
    float temperatura = sensor.readTemperature();
    float umidade = sensor.readHumidity();
    ruido = analogRead(ruidoPin);

    if(isnan(temperatura) || isnan(umidade)){
        Serial.println("Falhou para ler do Sensor DHT!");
        return;
    }

    if(!mqttClient.connected()){
        ConectarMQTT();
    }

    if(mqttClient.connected()){
        mqttClient.loop();
        digitalWrite(led, HIGH);
        mqttClient.publish("/imd0902/ESP32/Temperatura", String(temperatura).c_str(), true);
        Serial.print("Temperatura: ");
        Serial.println(temperatura);
        delay(1000);
        digitalWrite(led, LOW);
        mqttClient.publish("/imd0902/ESP32/Umidade", String(umidade).c_str(), true);
        Serial.print("Umidade: ");
        Serial.println(umidade);
        delay(1000);
        mqttClient.publish("/imd0902/ESP32/Ruido", String(ruido).c_str(), true);
        Serial.print("Ruido: ");
        Serial.println(Ruido);
        delay(1000);
    }
}
