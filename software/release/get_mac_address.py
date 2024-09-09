import serial.tools.list_ports


def get_serial_number(port_name):
    for port in serial.tools.list_ports.comports():
        if port.device == port_name:
            return port.serial_number


serial_number = get_serial_number("/dev/ttyACM1")
if serial_number:
    print(serial_number)
else:
    print("Error: Device not found")
