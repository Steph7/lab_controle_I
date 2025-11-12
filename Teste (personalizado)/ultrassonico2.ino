////// Configuração de Pinos 
//// Sensor ultrassônico

// Traseiro
const int TRIG_tras = 5;
const int ECHO_tras = 4;
// Dianteiro
const int TRIG_frente = 3;
const int ECHO_frente = 2;

// Obs.: confirmar se são mesmo da frente e de trás

/// Variáveis auxiliares
const int distancia_obstaculo = 20;

// ---- SET UP -----

void setup() {
  Serial.begin(9600);
  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
}

// ---- LOOP -----

void loop() {
  int distancia_frente = medir_distancia(TRIG_frente,ECHO_frente);
  int distancia_tras = medir_distancia(TRIG_tras,ECHO_tras);

    if((distancia_frente > distancia_obstaculo) && (distancia_tras > distancia_obstaculo)){
        Serial.print("Próximo obstáculo --> (frente): ");
        Serial.print(distancia_frente);
        Serial.println("cm | (trás): ");
        Serial.print(distancia_tras);
    }
    else if((distancia_frente <= distancia_obstaculo) && (distancia_tras > distancia_obstaculo)){
        Serial.print("ALERTA (frente): ");
        Serial.print(distancia_frente);
        Serial.println("cm");
    }
    else if((distancia_frente > distancia_obstaculo) && (distancia_tras <= distancia_obstaculo)){
        Serial.print("ALERTA (trás): ");
        Serial.print(distancia_tras);
        Serial.println("cm");
    }
    else{
      Serial.print("ALERTA!!! --> (frente): ");
        Serial.print(distancia_frente);
        Serial.print(" | (trás): ");
        Serial.print(distancia_tras);
    }
  delay(100);
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