from pymodbus.client import ModbusTcpClient
import time

# Cada vez que a internet do celular é roteada, é necessário atualizar o IP.
# Tanto aqui, quanto na IHM.
client = ModbusTcpClient("10.242.75.200", port=502)

if client.connect():
    print("Conectado ao ESP8266!")
    try:
        while True:
            result = client.read_holding_registers(0, 3)
            if result.isError():
                print("Erro ao ler registrador")
            else:
                print("--------------------------------------")
                print("Distância Frente:", result.registers[0])
                print("Distância Trás:", result.registers[1])
                print("Velocidade:", result.registers[2])
                print("--------------------------------------")
            time.sleep(1)
    except KeyboardInterrupt:
        client.close()
else:
    print("Falha na conexão")