import sensor_module_socket as sensor

server = sensor.Server(1234)
server.connect()
data = server.recvData()
print(data)
