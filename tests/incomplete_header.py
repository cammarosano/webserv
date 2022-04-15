import socket

msg = 	"GET / HTTP/1.1\r\n" \
		"Host: localhost:3000"

HOST = "127.0.0.1"
PORT = 3000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.send(msg.encode())
data = s.recv(1024)

print(f"Received data: \n{data.decode()}")
s.close()
 