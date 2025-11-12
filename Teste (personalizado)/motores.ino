////// Configuração de Pinos 
//// Módulo Ponte H - L298N
// Motor Direito
const int motor_dir_IN1 = 7;
const int motor_dir_IN2 = 8;
const int motor_dir_ENA = 9; // PWM

// Motor Esquerdo
const int motor_esq_IN3 = 12;
const int motor_esq_IN4 = 13;
const int motor_esq_ENB = 11; // PWM

////// Variáveis auxiliares
const int distancia_seguranca = 30;
int velocidade = 0; 
int direcao = 0; 
int tempoTotal = 100;

// ---- SET UP -----
void setup() {
  Serial.begin(9600);
  pinMode(motor_dir_ENA, OUTPUT); 
  pinMode(motor_esq_ENB, OUTPUT); 
  zerarSentido();
}

// ---- LOOP -----
void loop() {
  movimentoCronometrado(1,200);

  // Se quiser testar a função ajustarVelocidade() 
  // é só juntar com o arquivo ultrassonico2.ino

  delay(1000)
}

// ---- Funções -----

void definirVelocidade(int novo_vel) {
  if(novo_vel > 255){
    novo_vel = 255;
  }
  else if(novo_vel < 0){
    novo_vel = 0;
  }

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
  //delay(50);
}

void moverTras(){
  digitalWrite(motor_dir_IN1, LOW);
  digitalWrite(motor_dir_IN2, HIGH);
  digitalWrite(motor_esq_IN3, LOW);
  digitalWrite(motor_esq_IN4, HIGH);
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

void definirSentido(int direcao){
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
}

void definirMovimento(int novoSentido, int velocidade) {
  // Tem que reduzir a velocidade até parar ou até quase parar
  definirVelocidade(0);

  definirSentido(novoSentido);
  delay(100);

  definirVelocidade(velocidade);
  delay(1000);
}

void movimentoCronometrado(int novoSentido, int velocidadeMax){
  unsigned long tempoInicio = millis();
  unsigned long tempoFim = tempoInicio + tempoTotal * 1000UL;
  unsigned long agora;

  if (direcao != novoSentido){
    definirVelocidade(0); 
    definirSentido(novoSentido);
    direcao = novoSentido;
  }

  if(velocidadeMax > 255){
    velocidadeMax = 255;
  }
  else if(velocidadeMax < 0){
    velocidadeMax = 0;
  }
  
  while((agora = millis()) < tempoFim){
    float tempoPassado = (float)(agora - tempoInicio) / 1000.0; // em segundos
    float proporcao = tempoPassado / (float)tempoTotal;  
    
    int velocidadeAtual;
    
    if(proporcao < 0.2){ 
      // Aceleração
      velocidadeAtual = velocidade + (velocidadeMax - velocidade) * pow((proporcao / 0.2), 2);
    }
    else if(proporcao < 0.8){ 
      // Velocidade constante
      velocidadeAtual = velocidadeMax;
    }
    else{ 
      // Desaceleração
      velocidadeAtual = velocidadeMax * sqrt(((1.0 - proporcao) / 0.2));
    }

    definirVelocidade(velocidadeAtual);

    velocidade = velocidadeAtual;
  }

  if(velocidade != 0){
    definirVelocidade(0);
  }
}

void desligarMotor(){
  definirMovimento(0,0);
  delay(50);
}

void ajustarVelocidade(int dist_frente, int dist_tras){
  int delta_D;
  int novaVelocidade;
  if (direcao == 1){
    delta_D = dist_frente - distancia_seguranca;
  }
  else if (direcao == 2){
    delta_D = dist_tras - distancia_seguranca;
  }

  if((delta_D >=0) && (delta_D < 3)){
    // manter velocidade constante
    movimentoCronometrado(direcao, velocidade);
  }
  else{
    if(delta_D >= 3){
      // inverter sentido de direcao
      if (direcao == 1){
        direcao = 2;
        delta_D = dist_tras - distancia_seguranca;
      }
      else if (direcao == 2){
        direcao = 1;
        delta_D = dist_frente - distancia_seguranca;
      }
    }

    // Delta_D deve ser inversamente proporcional a Delta_V
    // Essa equação não considera o tempo gasto para a mudança de velocidade
    novaVelocidade = (1.0/abs(delta_D)) * velocidade;

    movimentoCronometrado(direcao, novaVelocidade);
  }
}