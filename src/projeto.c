#include <WiFi.h> // Biblioteca que permite com que o ESP32 se conecte a rede Wifi.
#include <WiFiClient.h> // 
#include <PubSubClient.h>
#include <stdio.h>

WiFiClient ESPWiFiClient;
PubSubClient mqttClient(ESPWiFiClient);

const char* wifi_ssid = "MySql"; // Nome da rede Wifi.
const char* wifi_password = ""; // Senha da rede Wifi.
int wifi_timeout = 2000000;

const char* mqtt_broker = "broker.hivemq.com";
const int mqtt_port = 1883;
int mqtt_timeout = 10000;

int led = 2; // So pra testar o funcionamento no ESP32, acender o LED 2.


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

void setup(){
    Serial.begin(115200);
    pinMode(led, OUTPUT);
    ConectarWifi();

    if(WiFi.status() == WL_CONNECTED){
      Serial.println("Conectando ao broker MQTT ..");
      mqttClient.setServer(mqtt_broker, mqtt_port);
    }
}

void loop(){
    if(!mqttClient.connected()){
      ConectarMQTT();
    }

    if(mqttClient.connected()){
      mqttClient.loop();
      digitalWrite(led, HIGH);
      mqttClient.publish("/imd0902/pratica03/status", String(1).c_str(), true);
      delay(5000);
      digitalWrite(led, LOW);
      mqttClient.publish("/imd0902/pratica03/status", String(0).c_str(), true);
      delay(2000);
    } 
}
