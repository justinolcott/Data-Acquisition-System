import socket
import pickle
import numpy as np

HEADERSIZE = 10
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((socket.gethostname(), 1234))

value = True
full_msg = b''
new_msg = True
while value:
  msg = s.recv(2048)
  if new_msg:
    msglen = int(msg[:HEADERSIZE])
    new_msg = False
  full_msg += msg
  if len(full_msg)-HEADERSIZE == msglen:
    d = pickle.loads(full_msg[HEADERSIZE:])
    new_msg = True
    full_msg = b''
    value = False
print(d)
np.savetxt("out.csv", d, delimiter=",")
