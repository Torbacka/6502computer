import warnings
from time import sleep

import serial
import serial.tools.list_ports


def find_arduinos(serial_number=None):
    """
    Trying to find arduino device in a "smart" way.
    :param serial_number: optional parameter for specifying a certain port to connect to.
    :return: Serial class
    """
    if serial_number is None:
        arduino_devices = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'arduino' in p.description.lower()
        ]
        if not arduino_devices:
            # Was not able to find a port named arduino
            arduino_devices = [port.device for port in serial.tools.list_ports.comports()]
        if len(arduino_devices) > 1:
            warnings.warn('Multiple Arduinos found - using the first')
        print("Connecting to: " + ", ".join(arduino_devices))
        return [connect(device) for device in arduino_devices]
    else:
        for port_info in serial.tools.list_ports.comports():
            if port_info.serial_number == serial_number:
                return [connect(port_info.device)]
        raise IOError("Could not find an arduino - is it plugged in?")


def connect(device):
    try:
        return serial.Serial(device, baudrate=57600, rtscts=1)
    except (IOError, serial.serialutil.SerialException) as e:
        print(f"I/O error({e.errno}, {e.strerror}")
        return None


def write_to_arduino(text):
    for arduino in find_arduinos():
        try:
            arduino.write(text)
            arduino.flush()
        except (IOError, serial.serialutil.SerialException) as e:
            print(f"I/O error({e.errno}, {e.strerror}")


if __name__ == "__main__":
    with open("program.bin", "rb") as program:
        write_to_arduino(program.read())

