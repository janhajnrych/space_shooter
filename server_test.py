import socket
import time

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)
msg = ["ping", "pong"]
counter = 0
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    print("listen")
    s.listen()
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        while True:
            print("x")
            in_data = conn.recv(1024)
            if in_data:
                print(in_data)
            counter = int(not bool(counter))
            out_str = msg[counter]
            conn.sendall(str.encode(out_str))
            time.sleep(0.25)
