import sys
from time import monotonic
import board
import busio
from adafruit_extended_bus import ExtendedI2C as I2C
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.ads1x15 import Mode
from adafruit_ads1x15.analog_in import AnalogIn
import numpy as np

#Default values
BUS = 4
GAIN = 16
RATE = 860
MODE = Mode.CONTINUOUS
I2CRATE = 1000000
ADDRESS = 0x48
PINS = 4
SAMPLESIZE = 860

def defaults():
  return BUS, GAIN, RATE, MODE, ADDRESS, PINS

def createI2C(bus):
  return I2C(bus, I2CRATE)

def createADS(i2c, gain, rate, mode, address):
  return ADS.ADS1115(i2c, gain, rate, mode, address)

def createChannel(ads, pins):
  if pins <= 3:
    return AnalogIn(ads, pins)
  if pins == 4:
    return AnalogIn(ads, 0, 1)
  if pins == 5:
    return AnalogIn(ads, 0, 3)
  if pins == 6:
    return AnalogIn(ads, 1, 3)
  if pins == 7:
    return AnalogIn(ads, 2, 3)

class Sensors():
  def __init__(self):
    self.channels = []
    self.size = 0

  def addChannel(self, bus, gain, rate, mode, address, pins):
    self.channels.append([bus, gain, rate, mode, address, pins])
    self.size += 1

  def addDefaultChannel(self):
    self.addChannel(BUS, GAIN, RATE, MODE, ADDRESS, PINS)

  def deleteChannel(self, index):
    del self.channels[index]
    self.size -= 1

  def createChannel(self, index):
    bus = self.channels[index][0]
    gain = self.channels[index][1]
    rate = self.channels[index][2]
    mode = self.channels[index][3]
    address = self.channels[index][4]
    pins = self.channels[index][5]
    chan = createChannel(createADS(createI2C(bus), gain, rate, mode, address), pins)
    return chan

  def createAll(self):
    channels = []
    for i in range(self.size):
      channels.append(self.createChannel(i))
    return channels

  def readAllVoltages(self, channels):
    voltages = []
    for i in range(self.size):
      voltages.append(channels[i].voltage)
    return voltages

  def getSamples(self, samples, start, channels):
    data = np.empty((samples, 1 + self.size))
    for i in range(samples):
      now = monotonic() - start
      voltages = self.readAllVoltages(channels)
      data[i][0] = monotonic() - start
      for j in range(self.size):
        data[i][1 + j] = voltages[j]
    print(data)
    return data


  def get(self):
    return self.channels[0].voltage
