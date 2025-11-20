////// Pacotes ou Bibliotecas
#include "SoftwareSerial.h"
#include <ModbusIP_ESP8266.h>

// ----------------------------------------

////// Configuração de Pinos 
//// Sensor ultrassônico
// Traseiro
const int TRIG_tras = 5;
const int ECHO_tras = 4;
// Dianteiro
const int TRIG_frente = 3;
const int ECHO_frente = 2;

//// Módulo Wifi
SoftwareSerial ESP_Serial(10, 11); // RX, TX

//// Módulo Ponte H - L298N
// Motor Direito
const int motor_dir_IN1 = 7;
const int motor_dir_IN2 = 8;
const int motor_dir_ENA = 9; // PWM

// Motor Esquerdo
const int motor_esq_IN3 = 12;
const int motor_esq_IN4 = 13;
const int motor_esq_ENB = 11; // PWM


// --------------------------------

////// Variáveis auxiliares
const int distancia_seguranca = 30;
int velocidade = 0; // começa parado
int direcao = 0; // começa no neutro (sem sentido)
int tempoTotal = 100; // segundos

// --------------------------------

////// OUTROS
// Configurações da rede Wi-Fi
const char* rede = "Nome-Rede-WiFi"; // <----- trocar aqui
const char* senha = "Senha-WiFi";

// Modbus
ModbusIP mb;

// Endereços dos registradores (do Elipse)
#define REG_DISTANCIA_FRENTE 0;  // Endereço 40001
#define REG_DISTANCIA_TRAS 1;    // Endereço 40002
#define REG_DIRECAO 2;           // Endereço 40003
#define REG_VELOCIDADE 3;        // Endereço 40004
#define REG_TEMPO 4;             // Endereço 40005
#define REG_COMANDO 5;           // Endereço 40006
#define REG_ERRO 6;              // Endereço 40007

// --------------------------------


//////// SET UP
void setup() {
  Serial.begin(115200);
  // Ultrassônico
  pinMode(TRIG_frente,OUTPUT); pinMode(ECHO_frente,INPUT); // Frente
  pinMode(TRIG_tras,OUTPUT); pinMode(ECHO_tras,INPUT);     // Trás

  // Ponte H / Motores
  pinMode(motor_dir_IN1, OUTPUT); pinMode(motor_dir_IN2, OUTPUT); pinMode(motor_dir_ENA, OUTPUT); // Motor Direito
  pinMode(motor_esq_IN3, OUTPUT); pinMode(motor_esq_IN4, OUTPUT); pinMode(motor_esq_ENB, OUTPUT); // Motor Esquerdo

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
  mb.addHreg(REG_VELOCIDADE);
  mb.addHreg(REG_TEMPO);
  mb.addHreg(REG_DIRECAO);
  mb.addHreg(REG_COMANDO);
  mb.addHreg(REG_ERRO);
}

// --------------------------------

//////// LOOP
void loop() {
  int distancia_frente = medir_distancia(TRIG_frente,ECHO_frente);
  int distancia_tras = medir_distancia(TRIG_tras,ECHO_tras);

  // Teste WiFi
  mb.Hreg(REG_DISTANCIA_FRENTE, distancia_frente);
  mb.Hreg(REG_DISTANCIA_TRAS, distancia_tras);
  //mb.Hreg(REG_VELOCIDADE, velocidade);
  //mb.Hreg(REG_DIRECAO, direcao);

  mb.task(); // se não funcionar, testar mb.poll()

  // Inserir lógica de controle aqui!

  //delay(1000);
}

// --------------------------------

//////// FUNÇÕES
int medir_distancia(int pinotrig,int pinoecho){
  digitalWrite(pinotrig,LOW);
  delayMicroseconds(2);
  digitalWrite(pinotrig,HIGH);
  delayMicroseconds(10);
  digitalWrite(pinotrig,LOW);

  return pulseIn(pinoecho,HIGH)/58;
}

void moverFrente(){
  digitalWrite(motor_dir_IN1, LOW);
  digitalWrite(motor_dir_IN2, HIGH);
  digitalWrite(motor_esq_IN3, LOW);
  digitalWrite(motor_esq_IN4, HIGH);
  //delay(50);
}

void moverTras(){
  digitalWrite(motor_dir_IN1, HIGH);
  digitalWrite(motor_dir_IN2, LOW);
  digitalWrite(motor_esq_IN3, HIGH);
  digitalWrite(motor_esq_IN4, LOW);
  //delay(50);
}

void zerarSentido(){
  digitalWrite(motor_dir_IN1, LOW);
  digitalWrite(motor_dir_IN2, LOW);
  digitalWrite(motor_esq_IN3, LOW);
  digitalWrite(motor_esq_IN4, LOW);
  //delay(50);
}

void freadaAbrupta(){
  digitalWrite(motor_dir_IN1, HIGH);
  digitalWrite(motor_dir_IN2, HIGH);
  digitalWrite(motor_esq_IN3, HIGH);
  digitalWrite(motor_esq_IN4, HIGH);
  //delay(50);
}

void definirSentido(int novoSentido){
  if (direcao = 0){
    // Deixar no neutro
    zerarSentido();
  } 
  else if(direcao == 1){
  // Frente
    moverFrente();
  }
  else if (direcao == 2){
  // Trás (Ré)
    moverTras();
  }
  else if (direcao == 3){
  // Frear bruscamente
    freadaAbrupta();
  }
  else{
    //mb.Hreg(REG_ERRO);
  }
}