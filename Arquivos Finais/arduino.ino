////// Pacotes ou Bibliotecas
#include "SoftwareSerial.h"
#include <PID_v1_bc.h> 

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
SoftwareSerial esp(10, 11); // RX, TX

//// Módulo Ponte H - L298N
// Motor Direito
const int motor_dir_IN1 = 7;
const int motor_dir_IN2 = 8;
const int motor_dir_ENA = 9; // PWM

// Motor Esquerdo
const int motor_esq_IN3 = 12;
const int motor_esq_IN4 = 13;
const int motor_esq_ENB = 6; // PWM


// --------------------------------

////// Variáveis auxiliares
const int distancia_seguranca = 5;
int velocidade = 0; // começa parado
int direcao = 0; // começa no neutro (sem sentido)
int tempoTotal = 100; // segundos

// --------------------------------
////// Variáveis e Objeto PID 
double Setpoint, Input=0, Output=0;
double Kp=1, Ki=0.9072, Kd=0; // Ajustar conforme necessário

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE); // Usamos REVERSE: se a distância diminuir, reduzir a velocidade

// --------------------------------

String bufferEsp = "";
String bufferSerial = "";

// --------------------------------


//////// SET UP
void setup() {
  Serial.begin(9600);
  esp.begin(9600);
  // Ultrassônico
  pinMode(TRIG_frente,OUTPUT); pinMode(ECHO_frente,INPUT); // Frente
  pinMode(TRIG_tras,OUTPUT); pinMode(ECHO_tras,INPUT);     // Trás

  // Ponte H / Motores
  pinMode(motor_dir_IN1, OUTPUT); pinMode(motor_dir_IN2, OUTPUT); pinMode(motor_dir_ENA, OUTPUT); // Motor Direito
  pinMode(motor_esq_IN3, OUTPUT); pinMode(motor_esq_IN4, OUTPUT); pinMode(motor_esq_ENB, OUTPUT); // Motor Esquerdo

  // Configura o PID
  Setpoint = (double)distancia_seguranca; 
  myPID.SetOutputLimits(0, 255); // Limites de saída (velocidade do motor)
  myPID.SetMode(AUTOMATIC); // Liga o PID
}

// --------------------------------

//////// LOOP
void loop() {
  int distancia_frente = medir_distancia(TRIG_frente,ECHO_frente);
  int distancia_tras = medir_distancia(TRIG_tras,ECHO_tras);

  if (esp.available()) {
    char c = esp.read();
    if (c == '\n' || c == '\r') {
      if (bufferEsp.length() > 0) {
        Serial.println("Do ESP: " + bufferEsp);
        bufferEsp = "";
      }
    } else {
      bufferEsp += c;
    }
  }

  // Lê do Serial USB
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (bufferSerial.length() > 0) {
        esp.println(bufferSerial);
        bufferSerial = "";
      }
    } else {
      bufferSerial += c;
    }
  }
  
  moverTras();

  // Mapeia a leitura para Input do PID
  Input = (double)distancia_frente;
  myPID.Compute(); // Calcula a velocidade 
  definirVelocidade(Output); // Define a velocidade dos motores conforme saída do PID
  
  //Serial.print("R0: ");
  //Serial.println(String(distancia));
  esp.print("R0:" + String(distancia_frente) + ",");
  esp.print("R1:" + String(distancia_tras) + ",");
  esp.print("R2:" + String(Output));
  esp.println();
  delay(100); 
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

void definirVelocidade(double novaVelocidade){
    int velocidadePWM = (int)novaVelocidade;

    // Velocidade não pode ultrapassar limite 
    if (velocidadePWM > 255){
      velocidadePWM = 255;
    }
    else if (velocidadePWM < 0){
      velocidadePWM = 0;
    } 

    // PID ajusta apenas a velocidade. Sentido é definido separadamente. 
    // Aplica a nova velocidade PWM aos motores
    analogWrite(motor_dir_ENA, velocidadePWM);
    analogWrite(motor_esq_ENB, velocidadePWM);
}