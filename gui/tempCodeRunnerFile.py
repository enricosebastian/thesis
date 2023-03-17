import serial.tools.list_ports

serial_instance = serial.Serial()

serial_instance.port = "COM3"
serial_instance.baudrate = 9600
serial_instance.open()


while True:
    if serial_instance.in_waiting:
        character = serial_instance.readline()
        print(character.decode('utf'))

serial_instance.close()

print("Hello")