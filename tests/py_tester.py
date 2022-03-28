import socket

msg = 	b"GET / HTTP/1.1\r\n " \
		b"host: localhost\r\n" \
		b"\r\n"

HOST = "127.0.0.1"
PORT = 3000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.sendall(msg)
data = s.recv(1024)

print(f"Received data: {data}")
s.close()
 