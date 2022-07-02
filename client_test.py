import socket
import time

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 65432  # The port used by the server

msg = ["ping", "pong"]
counter = 0
N = 1000
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    for i in range(N):
        print("x")
        in_data = s.recv(1024)
        if in_data:
            print(in_data)
        counter = int(not bool(counter))
        out_str = msg[counter]
        s.sendall(str.encode(out_str))
        time.sleep(0.25)
