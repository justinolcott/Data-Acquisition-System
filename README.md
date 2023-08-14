#### Work in Progress
# Data Acquisition System
- A Data Acquisition System currently in development to be cheaper and smaller than common DAQs.
- It features analog-to-digital sensors using an ADS1115 along with a quarter-Wheatstone bridge for strain gauges and other sensors such as pressure, temperature, GPS, accelerometer, and more which will be used for flight testing.

## Current
- Rev F features the most recent implementation of the Data Acquisition system that features an Analog-to-Digital sensor using ADS1115 and other sensors such as pressure, temperature, GPS, accelerometer, and more which will be used for flight testing.
- It is NOT out-of-the-box and required some serious tinkering to work, but it was eventually used as a piggyback to a much larger, expensive DAQ for testing a Blackhawk Helicopter. The sensors were wired using a breadboard and connected to a Raspberry Pi 4.

## Work in Progress
- The current work in progress is much simpler and is just a PCB hat for a Raspberry Pi Zero W. The PCB features an ADS1115, an adjustable quarter-Wheatstone bridge. It then outputs serial data to be read by a PC.

## Future
1. PCB for Raspberry Pi Zero that features:
    - high and low pass filters
    - ADS1120 for SPI and faster sampling
    - an integrated serial output via USB
2. PCB for ESP32 with the same features.
3. Adding more channels.
4. Adding different types of sensors:
    - GPS
    - Accelerometer
    - Absolute Pressure
    - Relative Pressure
    - Temperature
5. Interconnectivity between different controllers
    - Allows for multiple DAQs to be integrated to create a single large DAQ. The main issues will be managing time synchronization and data bandwidth.
6. Multiple Output types such as WebSockets to allow wireless streaming.

The vision is an affordable, yet high-quality DAQ with easy setup and usability that can be used for flight testing and a variety of other testing environments.
