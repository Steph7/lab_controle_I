from pymodbus.client.sync import ModbusTcpClient
import time

# IP e porta do ESP32
ESP32_IP = '192.168.0.50'
PORT = 502

# Criando cliente Modbus TCP
client = ModbusTcpClient(ESP32_IP, port=PORT)

# Endereços dos registradores
REG_VELOCIDADE = 0
REG_POSICAO_FRENTE = 1
REG_POSICAO_TRAS = 2

try:
    while True:
        # Lê os registradores holding (1 registrador cada)
        vel = client.read_holding_registers(REG_VELOCIDADE, 1).registers[0]
        frente = client.read_holding_registers(REG_POSICAO_FRENTE, 1).registers[0]
        tras = client.read_holding_registers(REG_POSICAO_TRAS, 1).registers[0]

        print(f"Velocidade: {vel}, Frente: {frente}, Tras: {tras}")

        time.sleep(1)  # lê a cada 1 segundo

except KeyboardInterrupt:
    print("Finalizando cliente")
    client.close()
