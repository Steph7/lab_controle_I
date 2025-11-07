from pymodbus.client.sync import ModbusTcpClient
import time

# IP e porta do ESP32
ESP32_IP = '192.168.0.50'
PORT = 502

# Criando cliente Modbus TCP
client = ModbusTcpClient(ESP32_IP, port=PORT)

# Endereços dos registradores
REG_DISTANCIA_FRENTE = 0;  
REG_DISTANCIA_TRAS = 1;    
REG_DIRECAO = 2;        
REG_VELOCIDADE = 3;   
REG_TEMPO = 4;             
REG_COMANDO = 5;           
REG_ERRO = 6;              

try:
    while True:
        # Ler os registradores holding
        vel = client.read_holding_registers(REG_VELOCIDADE, 1).registers[0]
        frente = client.read_holding_registers(REG_POSICAO_FRENTE, 1).registers[0]
        tras = client.read_holding_registers(REG_POSICAO_TRAS, 1).registers[0]
        direcao = client.read_holding_registers(REG_DIRECAO, 1).registers[0]

        print(f"Velocidade: {vel}, Dist_Frente: {frente}, Dist_Tras: {tras}, Direção: {direcao} \n")

        time.sleep(1)  # lê a cada 1 segundo

except KeyboardInterrupt:
    print("Finalizando cliente")
    client.close()
