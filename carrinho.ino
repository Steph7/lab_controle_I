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

// --------------------------------

////// OUTROS
// Configurações da rede Wi-Fi
const char* rede = "Nome-Rede-WiFi";
const char* senha = "Senha-WiFi";

// Modbus
ModbusIP mb;

// Endereços dos registradores (do Elipse)
#DEFINE REG_DISTANCIA_FRENTE 0;  // Endereço 40001
#DEFINE REG_DISTANCIA_TRAS 1;    // Endereço 40002
#DEFINE REG_DIRECAO 2;           // Endereço 40003
#DEFINE REG_VELOCIDADE 3;        // Endereço 40004
#DEFINE REG_COMANDO 4;           // Endereço 40005
#DEFINE REG_ERRO 5;              // Endereço 40006

// --------------------------------


//////// SET UP
void setup() {
  Serial.begin(115200);
  // Ultrassônico
  pinMode(TRIG_frente,OUTPUT);
  pinMode(ECHO_frente,INPUT);

  pinMode(TRIG_tras,OUTPUT);
  pinMode(ECHO_tras,INPUT);

  // Ponte H / Motores
  pinMode(motor_dir_IN1, OUTPUT); pinMode(motor_dir_IN2, OUTPUT); pinMode(motor_dir_ENA, OUTPUT); // Motor Direito
  pinMode(motor_esq_IN3, OUTPUT); pinMode(motor_esq_IN4, OUTPUT); pinMode(Emotor_esq_NB, OUTPUT); // Motor Esquerdo

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
  mb.server();
  mb.addHreg(REG_DISTANCIA_FRENTE);
  mb.addHreg(REG_DISTANCIA_TRAS);
  mb.addHreg(REG_VELOCIDADE);
  mb.addHreg(REG_DIRECAO);
  mb.addHreg(REG_COMANDO);
  mb.addHreg(REG_ERRO);
  
}

// --------------------------------

//////// LOOP
void loop() {
  int distancia_frente = medir_distancia(TRIG_frente,ECHO_frente);
  int distancia_tras = medir_distancia(TRIG_tras,ECHO_tras);
  int velocidade = 0; // atualizar depois
  int direcao = 1; // pode ser 1 (frente) ou 2 (trás)

  // Teste WiFi
  mb.Hreg(REG_DISTANCIA_FRENTE, distancia_frente);
  mb.Hreg(REG_DISTANCIA_TRAS, distancia_tras);
  //mb.Hreg(REG_VELOCIDADE, velocidade);
  //mb.Hreg(REG_DIRECAO, direcao);

  mb.task(); 

  // Essa parte aqui depois que ele se tornar autônomo, nós vamos tirar!!!
  // Mas por enquanto, vamos comandá-lo via Modbus
  int comando = mb.Hreg(REG_COMANDO);
  int nova_velocidade = mb.Hreg(REG_VELOCIDADE);
  int nova_direcao = mb.Hreg(REG_DIRECAO);
  
  if (nova_velocidade != velocidade) {
    velocidade = nova_velocidade;
  }
  mb.Hreg(REG_VELOCIDADE, velocidade);

  if (nova_direcao != direcao) {
    direcao = nova_direcao;
  }
  mb.Hreg(REG_DIRECAO, direcao);

  // COMANDOS vindos do cliente Modbus
  if (comando == 1) {
    // Liga motor frente ou ré
    definirMovimento(nova_direcao, nova_velocidade);
  } 
  else if (comando == 0) {
    // Desliga motor
    desligarMotor();
  }

  delay(1000);
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

void definirVelocidade(int v){
  // Intervalo aceito de 0–255
  analogWrite(motor_dir_ENA, v); 
  analogWrite(motor_esq_ENB, v); 
  delay(50);
}

void moverFrente(){
  digitalWrite(motor_dir_IN1, HIGH);
  digitalWrite(motor_dir_IN2, LOW);
  digitalWrite(motor_esq_IN3, HIGH);
  digitalWrite(motor_esq_IN4, LOW);
  delay(50);
}

void moverTras(){
  digitalWrite(motor_dir_IN1, LOW);
  digitalWrite(motor_dir_IN2, HIGH);
  digitalWrite(motor_esq_IN3, LOW);
  digitalWrite(motor_esq_IN4, HIGH);
  delay(50);
}

void desligarMotor(){
  digitalWrite(motor_dir_IN1, LOW);
  digitalWrite(motor_dir_IN2, LOW);
  digitalWrite(motor_esq_IN3, LOW);
  digitalWrite(motor_esq_IN4, LOW);
  delay(50);

  definirVelocidade(0);
  delay(50);
}

void definirMovimento(int direcao, int velocidade){
  if (direcao == 1){
  // Frente
    moverFrente();
  }
  else if (direcao == 2){
  // Trás (Ré)
    moverTras();
  }
  else{
    mb.Hreg(REG_ERRO);
  }
  definirVelocidade(velocidade);
  delay(1000);
}

int ajustarVelocidade(int dist_menor, int dist_maior){
  delta_D = dist_maior - dist_menor;
  // Delta_D deve ser inversamente proporcional a Delta_V
  novaVelocidade = (1/delta_D) * velocidade;
  return novaVelocidade;
}

void controlarVelocidade(int dist_frente, int dist_tras){
  if(dist_frente < distancia_seguranca){
    v = ajustarVelocidade(dist_frente, distancia_seguranca);
  }
  else{

  }
}

// --------------------------------
