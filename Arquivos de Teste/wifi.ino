/*******************************************************************************
* Master Kit - Hello Blynk
* Notificacao simples com o aplicativo Blynk IoT.
*******************************************************************************/

//Declaracao dos parametros de conexao do aplicativo
//Alterar os codigos abaixo de acordo com o que foi gerado na plataforma
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_AQUI"
#define BLYNK_DEVICE_NAME "NOME_DISPOSITIVO_AQUI"
#define BLYNK_AUTH_TOKEN "CODIGO_AUTENTICACAO_AQUI"

//Definicao do monitoramento de conexao da placa pela serial
#define BLYNK_PRINT Serial

//Adicao das bibliotecas
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

//Declaracao da variavel que armazena o codigo de autenticacao para conexao
char auth[] = BLYNK_AUTH_TOKEN;

//Declaracao do nome e senha da rede Wi-Fi
//Altere as variaveis abaixo com o nome e senha da sua rede Wi-Fi
char ssid[] = "SUA_REDE_WIFI_AQUI";
char pass[] = "SENHA_REDE_WIFI_AQUI";

//Criacao do objeto serial para comunicacao com o ESP8266
SoftwareSerial EspSerial(10, 11); // RX, TX

//Declaracao da variavel que armazena a velocidade de comunicacao do modulo
const int ESP8266_BAUD = 9600;

//Confiracao do objeto 'wifi' para usar a serial do ESP8266 para conexao
ESP8266 wifi(&EspSerial);

//Declaracao do pino conectado ao potenciometro
const int PINO_POT = A0;

//Funcao que le o pino V1 a cada atualizacao de estado
BLYNK_WRITE(V1){
  int pinValue = param.asInt(); //Le o valor do pino virtual
  digitalWrite(LED_BUILTIN, pinValue); //Aciona o LED da placa de acordo com o valor lido pelo pino virtual
  
}

//Configuracao do codigo
void setup(){

  //Inicializacao do monitor serial
  Serial.begin(9600);

  //Configura o pino conectado ao potenciometro como entrada
  pinMode(PINO_POT, INPUT);

  //Inicializa a comunicacao serial do ESP8266
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  //Inicializacao da comunicacao e conexao do modulo ao aplicativo
  Blynk.begin(auth, wifi, ssid, pass);

}

//Repeticao do codigo
void loop(){
  Blynk.run(); //Mantem a conexao ativa com o aplicativo e processa comandos recebidos ou enviados

  //Realiza a leitura do potenciometro
  int valorPot = analogRead(PINO_POT);
  //Verifica se a leitura do potenciometro e maior ou igual a 1020
  if (valorPot >= 1020) {
    Blynk.logEvent("pot"); //Envia uma nova entrada ao evento criado, para notificacao
  }
}