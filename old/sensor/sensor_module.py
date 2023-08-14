import sensor_module_socket as soc
import sensor_module_getting as sensor
import time
import socket
import numpy as np
import threading

#funtion that given a client socket will connect and send the sensor values
def runSensor(client):
  sensors = sensor.Sensors()
  sensors.addDefaultChannel()
  channels = sensors.createAll()
  start = time.monotonic()
  status = ''
  while status != "start":
    status = client.recvData()
  try:
    while True:
      data = sensors.getSamples(860, start, channels)
      client.sendData(data)
  except KeyboardInterrupt:
    print("keyboard interrupt")
    pass
  except BrokenPipeError:
    pass
  client.disconnectFromServer()
  print("done with client")

#function that'll loop until connected to a server
def connectToServer(serverAddress, port):
  status = True
  while status:
    try:
      client = soc.Client(serverAddress)
      client.connecttoserver(port)
      status = False
    except ConnectionRefusedError:
      status = True
    except OSError as e:
      if e.errno == 101:
        status = True
      else:
        status = False
    time.sleep(1) #slows down the program if there is nothing to connect to
  return client

#function that will runAll
def run(serverAddress, port):
  client = connectToServer(serverAddress, port)
  runSensor(client)

def startMultiplePorts(ports):
  servers = []
  for i in range(len(ports)):
    server = soc.Server(ports[i])
    server.connect()
    servers.append(server)
  return servers

def startSensors(servers):
  for i in range(len(servers)):
    servers[i].sendData("start")

def threadGetData(stop, server, file):
  f = open(file, "w+")
  f.close()
  f = open(file, "ab")
  try:
    while True:
      data = server.recvData()
      print(data)
      print(server)
      np.savetxt(f, data, fmt = '%5f', delimiter=",")
      global stop_threads
      if stop():
        break
  except KeyboardInterrupt:
    print("KB interrupt")
    pass
  except ValueError:
    print("value error")
    pass
  f.close()
  server.disconnect()

def runServer(ports, files):
  servers = startMultiplePorts(ports)
  stop_threads = False
  threads = []
  print("step 1")
  for i in range(len(servers)):
    print("step 1.5")
    thread = threading.Thread(target = threadGetData, args=(lambda : stop_threads, servers[i], files[i]))
    print("step 1.75")
    threads.append(thread)
    print("step 1.8")
  print("step 2")
  startSensors(servers)
  print("step 3")
  for i in range(len(threads)):
    threads[i].start()
  print("step 4")
  status = ''
  while status != "stop":
    print("step 5")
    status = input("type stop to stop")
  stop_threads = True
  for i in range(len(threads)):
    threads[i].join()
  print("done")
