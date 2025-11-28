# Trabalho Final de Laboratório de Controle I

Projeto L2-ZETA* da empresa A3GS.

Os códigos de Testes, têm os códigos trata cada módulo do carrinho de forma independente. 
O arquivo carrinho.ino reune os pontos fundamentais de cada módulo.

## Configuração de Pinos

Esquema de ligação de Pinos do Carrinho:
![Image](https://github.com/Steph7/lab_controle_I/blob/master/Carrinho%20(v3)%20-%20Trabalho%20Final.png?raw=true)

```
//// Sensor ultrassônico
// Traseiro
const int TRIG_tras = 5;
const int ECHO_tras = 4;
// Dianteiro
const int TRIG_frente = 3;
const int ECHO_frente = 2;

//// Módulo Ponte H - L298N
// Motor Direito
const int motor_dir_IN1 = 7;
const int motor_dir_IN2 = 8;
const int motor_dir_ENA = 9; // PWM

// Motor Esquerdo
const int motor_esq_IN3 = 12;
const int motor_esq_IN4 = 13;
const int motor_esq_ENB = 6; // PWM
```

## Sensores

### Ultrassônico
O sensor ultrassônico não funciona corretamente sem essa função.

```
int medir_distancia(int pinotrig,int pinoecho){
  digitalWrite(pinotrig,LOW);
  delayMicroseconds(2);
  digitalWrite(pinotrig,HIGH);
  delayMicroseconds(10);
  digitalWrite(pinotrig,LOW);

  return pulseIn(pinoecho,HIGH)/58;
}
```
A ser utilizada da seguinte forma:

```
int distancia_frente = medir_distancia(TRIG_frente,ECHO_frente);
int distancia_tras = medir_distancia(TRIG_tras,ECHO_tras);
```

A explicação está a seguir:

<img width="728" height="557" alt="Captura de Tela 2025-11-20 às 11 57 16" src="https://github.com/user-attachments/assets/4c2fe445-8c13-4c62-baca-0dd06449918c" />


### Motores
Os pinos do PWM são os que permitem regular a tensão aplicada nos motores. Os demais são os que determinam o sentido do movimento. Assim, as seguintes funções foram criadas para simplificar a determinação do sentido do movimento do carrinho.

São elas: moverFrente(), moverTras(), zerarSentido() e freadaAbrupta(). E têm o seguinte formato:

Obs.: O carrinho continua indo pra frente com a função moverTras(). *moonwalker

```
void moverTras(){
  digitalWrite(motor_dir_IN1, LOW);
  digitalWrite(motor_dir_IN2, HIGH);
  digitalWrite(motor_esq_IN3, LOW);
  digitalWrite(motor_esq_IN4, HIGH);
  //delay(50);
}
```
As demais funções que estão no arquivo motores.ino, não precisam ser utilizadas já que será criada uma lógica de controle diferente.

### Módulo Wifi

```
SoftwareSerial esp(10, 11); // RX, TX
```
Foram utilizadas os pinos 10 e 11 do Arduino, já que oferecem uma transmissão de dados com mais estabilidade.

Os dados estão sendo transmitidos via serial do Arduino para o ESP8266. Para fazer a conexão com a IHM (Elipse), foi utilizado Modbus TCP.

Obs.: Para que a conexão funcione sem problemas, é importante que a rede tenha o nome e a senha conforme descritos no código.


Obs.: *Apelidado carinhosamente de Zezetinha, em homenagem a avó da Amanda.
