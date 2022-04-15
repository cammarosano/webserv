import socket

msg = 	"GET / HTTP/1.1\r\n" \
		"Host: localhost:3000\r\n" \
		"\r\n"

HOST = "127.0.0.1"
PORT = 3000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.send(msg.encode())
data = s.recv(1024)

print(f"Received data: {data}")
s.close()
 