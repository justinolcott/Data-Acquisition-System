import socket
import pickle

HEADERSIZE = 10
BYTESIZE = 2048

def sendDataFunc(s, data):
  msg = pickle.dumps(data)
  msg = bytes(f'{len(msg):<{HEADERSIZE}}', "utf-8") + msg
  s.send(msg)

def recvDataFunc(s):
  try:
    value = True
    full_msg = b''
    new_msg = True
    while value:
      msg = s.recv(BYTESIZE)
      if new_msg:
        msglen = int(msg[:HEADERSIZE])
        new_msg = False
      full_msg += msg
      if len(full_msg)-HEADERSIZE == msglen:
        d = pickle.loads(full_msg[HEADERSIZE:])
        value = False
    return d
  except:
    return False

class Server:
  def __init__(self, port):
    self.sendAddress = ''
    self.port = port
    self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    self.s.bind((self.sendAddress, port))
    self.s.listen(10)

  def connect(self):
    self.clientsocket, self.recvAddress = self.s.accept()
    print(f"Connection from {self.recvAddress} made")

  def disconnect(self):
    self.s.close()

  def sendData(self, data):
    sendDataFunc(self.clientsocket, data)

  def recvData(self):
    return recvDataFunc(self.clientsocket)

class Client:
  def __init__(self, serverAddress):
    self.serverAddress = serverAddress
    self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

  def connecttoserver(self, port):
    self.s.connect((self.serverAddress, port))

  def disconnectFromServer(self):
    self.s.close()

  def recvData(self):
    return recvDataFunc(self.s)

  def sendData(self, data):
    sendDataFunc(self.s, data)
