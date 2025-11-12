////// Pacotes ou Bibliotecas
#include "SoftwareSerial.h"
#include <ModbusIP_ESP8266.h>

//// Módulo Wifi
SoftwareSerial ESP_Serial(10, 11); // RX, TX

// Configurações da rede Wi-Fi
const char* rede = "Nome-Rede-WiFi"; // <----- trocar aqui
const char* senha = "Senha-WiFi";

// Modbus
ModbusIP mb;

// Endereços dos registradores (do Elipse)
#DEFINE REG_DISTANCIA_FRENTE 0;  // Endereço 40001
#DEFINE REG_DISTANCIA_TRAS 1;    // Endereço 40002

// Como configurar no Elipse: https://www.youtube.com/watch?v=ETj8CDtNQMA

// ---- SET UP -----

void setup() {
  Serial.begin(115200);
  // Ultrassônico
  pinMode(TRIG_frente,OUTPUT); pinMode(ECHO_frente,INPUT); // Frente
  pinMode(TRIG_tras,OUTPUT); pinMode(ECHO_tras,INPUT);     // Trás

  // Wifi
  WiFi.begin(rede, senha);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());

  // Inicia servidor Modbus
  mb.server(); // Porta Padrão 502
  mb.addHreg(REG_DISTANCIA_FRENTE);
  mb.addHreg(REG_DISTANCIA_TRAS);
}

// ---- LOOP -----

void loop() {
  int distancia_frente = medir_distancia(TRIG_frente,ECHO_frente);
  int distancia_tras = medir_distancia(TRIG_tras,ECHO_tras);

  // Teste WiFi
  mb.Hreg(REG_DISTANCIA_FRENTE, distancia_frente);
  mb.Hreg(REG_DISTANCIA_TRAS, distancia_tras);

  mb.task(); // se não funcionar, testar mb.poll()

  delay(1000);
}

// ---- Funções -----

int medir_distancia(int pinotrig,int pinoecho){
  digitalWrite(pinotrig,LOW);
  delayMicroseconds(2);
  digitalWrite(pinotrig,HIGH);
  delayMicroseconds(10);
  digitalWrite(pinotrig,LOW);

  return pulseIn(pinoecho,HIGH)/58;
}