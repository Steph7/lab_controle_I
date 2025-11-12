from pymodbus.client.sync import ModbusTcpClient
import time
import struct

# IP e porta do ESP32
moduloWifi_IP = '192.168.0.50'
PORT = 502

# Criando cliente Modbus TCP
client = ModbusTcpClient(moduloWifi_IP, port=PORT)

if not client.connect():
    print("Falha na conexão com Módulo Wifi")
    exit()

# Endereços dos registradores
REG_DISTANCIA_FRENTE = 0;  
REG_DISTANCIA_TRAS = 1;    
REG_DIRECAO = 2;        
REG_VELOCIDADE = 3;   
REG_TEMPO = 4;             
REG_COMANDO = 5;           
REG_ERRO = 6;              

def editarFormato(client, address, count=1, data_type='INT16', unit=1):
    #Lê registradores Modbus e converte para o tipo desejado
    
    resp = client.read_holding_registers(address, count, unit=unit)
    
    if resp.isError():
        return None 
    
    regs = resp.registers
    
    if data_type == 'INT16':
        valor = regs[0]
        if valor > 32767: 
            valor -= 65536
        return valor

    elif data_type == 'UINT16':
        return regs[0]

    elif data_type == 'INT32':
        b = struct.pack('>HH', regs[0], regs[1])
        return struct.unpack('>i', b)[0]

    elif data_type == 'UINT32':
        b = struct.pack('>HH', regs[0], regs[1])
        return struct.unpack('>I', b)[0]

    elif data_type == 'FLOAT32':
        b = struct.pack('>HH', regs[0], regs[1])
        return struct.unpack('>f', b)[0]

    elif data_type == 'STRING':
        b = b''.join(struct.pack('>H', r) for r in regs)
        return b.decode('ascii').rstrip('\x00')

    else:
        raise ValueError("Tipo de dado inválido")


try:
    while True:
    
        vel = editarFormato(client, REG_VELOCIDADE, data_type='INT16')
        frente = editarFormato(client, REG_DISTANCIA_FRENTE, data_type='INT16')
        tras = editarFormato(client, REG_DISTANCIA_TRAS, data_type='INT16')
        direcao = read_register(client, REG_DIRECAO, data_type='INT16')

        print(f"Velocidade: {vel}, Dist_Frente: {frente}, Dist_Tras: {tras}, Direção: {direcao} \n")

        time.sleep(1)  # lê a cada 1 segundo

except KeyboardInterrupt:
    print("Finalizando cliente")
    client.close()
