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

// --------------------------------

////// OUTROS
// Configurações da rede Wi-Fi
const char* rede = "Nome-Rede-WiFi"; // <----- trocar aqui
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
  pinMode(TRIG_frente,OUTPUT); pinMode(ECHO_frente,INPUT); // Frente
  pinMode(TRIG_tras,OUTPUT); pinMode(ECHO_tras,INPUT);     // Trás

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

  // COMANDOS vindos do cliente Modbus
  if (comando == 1) {
    // Definir sentido de rotação do motor 
    // (0 - neutro, 1 - frente, 2 - ré, 3 - freada)
    // Definir velocidade (valor de 0 a 255)
    definirMovimento(nova_direcao, nova_velocidade);
    mb.Hreg(REG_VELOCIDADE, velocidade);
    mb.Hreg(REG_DIRECAO, direcao);
  } 
  else if (comando == 2) {
    // Manter distância do objeto da frente
    
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

void definirVelocidade(int novo_vel) {
  if (velocidade > novo_vel) {
    for (int v = velocidade; v <= novo_vel; v++) {
      analogWrite(motor_dir_ENA, v);
      analogWrite(motor_esq_ENB, v);
      delay(10);
    }
  } else {
    for (int v = velocidade; v >= novo_vel; v--) {
      analogWrite(motor_dir_ENA, v);
      analogWrite(motor_esq_ENB, v);
      delay(10);
    }
  }

  velocidade = novo_vel; 
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

void zerarSentido(){
  digitalWrite(motor_dir_IN1, LOW);
  digitalWrite(motor_dir_IN2, LOW);
  digitalWrite(motor_esq_IN3, LOW);
  digitalWrite(motor_esq_IN4, LOW);
  delay(50);
}

void freadaAbrupta(){
  digitalWrite(motor_dir_IN1, HIGH);
  digitalWrite(motor_dir_IN2, HIGH);
  digitalWrite(motor_esq_IN3, HIGH);
  digitalWrite(motor_esq_IN4, HIGH);
  delay(50);
}

void definirSentido(int novoSentido){
  if (direcao != novoSentido){
    direcao = novoSentido;
  }

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
    mb.Hreg(REG_ERRO);
  }
}

void definirMovimento(int novoSentido, int velocidade) {
  // Tem que reduzir a velocidade até parar ou até quase parar
  definirVelocidade(0);

  definirSentido(novoSentido);
  delay(100);

  definirVelocidade(velocidade);
  delay(1000);
}

void desligarMotor(){
  definirMovimento(0,0);
  delay(50);
}

//////// AÇÃO DE CONTROLE (versão sem controlador)
/*
void ajustarVelocidade(int dist_menor, int dist_maior){
  delta_D = dist_maior - dist_menor;
  // Delta_D deve ser inversamente proporcional a Delta_V
  // Essa equação não considera o tempo gasto para a mudança de velocidade
  novaVelocidade = (1/delta_D) * velocidade;
  
  definirMovimento(novaVelocidade);

  velocidade = novaVelocidade;
}

void controlarVelocidade(int dist_frente, int dist_tras){
  if(dist_frente < distancia_seguranca){
    v = ajustarVelocidade(dist_frente, distancia_seguranca);
  }
  else{

  }
}
*/

// --------------------------------
