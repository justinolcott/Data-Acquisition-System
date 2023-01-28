import board
import shutil #used in saving to a usb
import numpy as np
from time import monotonic
import busio
from adafruit_extended_bus import ExtendedI2C as I2C 
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.ads1x15 import Mode
from adafruit_ads1x15.analog_in import AnalogIn
import socket
import sensor_module_socket as sensor
RATE = 860
SAMPLES = 1000

i2c = I2C(4, 1000000)
ads = ADS.ADS1115(i2c)
chan0 = AnalogIn(ads, ADS.P0, ADS.P1)
ads.mode = Mode.CONTINUOUS
ads.data_rate = RATE
ads.gain = 16
data = np.zeros((1, 2))
start = monotonic()
for i in range(SAMPLES):
  data = np.append(data, np.array([[monotonic() - start, chan0.voltage]]), axis=0)
end = monotonic()
print(end - start)
print(data)
#Sending the Object
client = sensor.Client(socket.gethostname())
client.connecttoserver(1234)
client.sendData(data)
#HEADERSIZE = 10
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.bind((socket.gethostname(), 1234))
#s.listen(10)

#value = True
#while value:
#  clientsocket, address = s.accept()
#  print(f"Connection from {address} has been established")
#  msg = pickle.dumps(data)
#  msg = bytes(f'{len(msg):<{HEADERSIZE}}', "utf-8") + msg
#  clientsocket.send(msg)
#  value = False
