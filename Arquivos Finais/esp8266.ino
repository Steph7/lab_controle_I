#include <ESP8266WiFi.h>
#include <ModbusTCP.h>

String buffer = "";

// Endereços dos registradores (do Elipse)
#define REG_DISTANCIA_FRENTE 0  // Endereço 40001
#define REG_DISTANCIA_TRAS 1    // Endereço 40002
#define REG_VELOCIDADE 2        // Endereço 40003

const char* ssid = "Zeta"; 
const char* pass = "testeModbus"; 

ModbusTCP mb;

void setup() {
  Serial.begin(9600);
  Serial.println("\nIniciando ESP8266...");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  mb.server();
  mb.addHreg(REG_DISTANCIA_FRENTE);
  mb.addHreg(REG_DISTANCIA_TRAS);
  mb.addHreg(REG_VELOCIDADE);
  Serial.println("Servidor Modbus iniciado.");
}

void loop() {
  // Leitura da serial
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (buffer.length() > 0) {
        processarStringModbus(buffer);
        buffer = "";
      }
    } else {
      buffer += c;
    }
  }

  mb.task(); 
}

void processarStringModbus(String linha) {
  linha.trim();  
  Serial.print("Recebido: '"); Serial.print(linha); Serial.print("'");

  String r0, r1, r2;

  int p1 = linha.indexOf(',');
  int p2 = linha.indexOf(',', p1 + 1);

  String item0 = linha.substring(0, p1);
  String item1 = linha.substring(p1 + 1, p2);
  String item2 = linha.substring(p2 + 1);

  item0.trim();
  item1.trim();
  item2.trim();

  r0 = item0.substring(item0.indexOf(':') + 1);
  r1 = item1.substring(item1.indexOf(':') + 1);
  r2 = item2.substring(item2.indexOf(':') + 1);

  r0.trim();
  r1.trim();
  r2.trim();

  int r0_int = r0.toInt();
  int r1_int = r1.toInt();
  int r2_int = r2.toInt();

  mb.Hreg(REG_DISTANCIA_FRENTE, r0_int); 
  //Serial.print(r0_int);
  mb.Hreg(REG_DISTANCIA_TRAS, r1_int);
  //Serial.print(r1_int);
  mb.Hreg(REG_VELOCIDADE, r2_int);
  //Serial.println(r2_int);
}
