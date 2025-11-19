// Código a ser colocado no ESP8266

////// Pacotes ou Bibliotecas
#include <ESP8266WiFi.h>
#include <ModbusIP_ESP8266.h>
// Baixar: https://github.com/emelianov/modbus-esp8266

// Configurações da rede Wi-Fi
const char* rede = "Nome-Rede-WiFi"; 
const char* senha = "Senha-WiFi";

// Modbus
ModbusIP mb;

// Endereços dos registradores (do Elipse)
#define REG_DISTANCIA_FRENTE 0;  // Endereço 40001
#define REG_DISTANCIA_TRAS 1;    // Endereço 40002

// Como configurar no Elipse: https://www.youtube.com/watch?v=ETj8CDtNQMA

// ---- SET UP -----

void setup() {
  Serial.begin(115200);

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
  // Lê dados do Arduino via Serial
  if (Serial.available()) {
    String dados = Serial.readStringUntil('\n'); // lê até a próxima linha
    int virgula = dados.indexOf(',');
    if (virgula > 0) {
      int distancia_frente = dados.substring(0, virgula).toInt();
      int distancia_tras = dados.substring(virgula + 1).toInt();

      // Atualiza Modbus
      mb.Hreg(REG_DISTANCIA_FRENTE, distancia_frente);
      mb.Hreg(REG_DISTANCIA_TRAS, distancia_tras);
    }
  }
  mb.task(); // se não funcionar, testar mb.poll()
}