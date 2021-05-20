import socket
import time
import random

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 65432        # The port used by the server

def parse_msg(msg):
    return [parse_one_msg(t) for t in msg.split(";")]
    
    
def parse_one_msg(msg):
    tokens = msg.split(":")
    if len(tokens) != 2:
        return None
    subject = tokens[0]
    tokens = tokens[1].split(" ")
    if len(tokens) != 5:
        return None
    data = {}
    try:
        data = {
           "who": subject,
           "life": int(tokens[0]),
           "x": int(tokens[1]),
           "y": int(tokens[2]),
           "vx": int(tokens[3]),
           "vy": int(tokens[4])
        }
    except ValueError as e:
        print(str(e))
        return None
    return data


command_list = ["a1", "d1"]
def run_client():
    i = 0
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(bytes('w1', encoding='utf-8'))
        time.sleep(0.1)
        s.sendall(bytes('k1', encoding='utf-8'))
        #s.sendall(bytes('t0', encoding='utf-8'))
        while True:
            s.sendall(bytes('k1', encoding='utf-8'))
            time.sleep(0.05)
            s.sendall(bytes('w1', encoding='utf-8'))
            time.sleep(0.1)
            s.sendall(bytes(random.choice(command_list), encoding='utf-8'))
            i+=1
            data = s.recv(1024)
            formated_msg = parse_msg(data.decode("utf-8"))
            print('Received', formated_msg)
            time.sleep(1.0)

if __name__ == '__main__':
    run_client()
