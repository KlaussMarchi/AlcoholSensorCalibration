import serial  # pip install pyserial
from time import sleep
import keyboard
import pandas as pd
import serial.tools.list_ports


def getValue():
    global device

    try:
        return eval(device.readline().decode('utf-8'))
    except:
        return None


def getPort():
    ports  = [port for port in serial.tools.list_ports.comports()]

    target = 0

    for i, port in enumerate(ports):
        if 'usb' in str(port).lower():
            target = i

    selected = str(ports[target]).split(' ')[0].strip()
    return selected


path   = 'DataBase.csv'
port   = getPort()
device = serial.Serial(port, 115200, timeout=10)
list   = []

print('conectado!')
sleep(2)

device.write('start\r\n'.encode())
print('aguarde...')


while True:
    values = getValue()
    
    if keyboard.is_pressed('enter'):
        break

    if values is None:
        continue
    
    list.append(values)
    print(values)

df = pd.DataFrame(list)
df.to_csv(path, index=False)