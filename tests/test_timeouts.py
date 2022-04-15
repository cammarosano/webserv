import requests, threading, socket, time

RED = "\033[0;31m"
RESET = "\033[0m"
GREEN = "\033[0;32m"


n_clients = 10

# sends an incomplete header, should time-out
def incomple_request(client_id):
	msg = 	"GET / HTTP/1.1\r\n" \
			"Host: localhost:3000"

	HOST = "127.0.0.1"
	PORT = 3000

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST, PORT))
	s.send(msg.encode())
	data = s.recv(1024)
	s.close()
	response_line = data.decode().split('\r\n')[0]
	# print(response_line)
	if (response_line == "HTTP/1.1 408 Request Timeout"):
		print(GREEN, "Request timeout OK", RESET)
	else:
		print(RED, "Request timeout NOK", RESET)


def cgi_time_out(client_id):
	r = requests.get("http://localhost:3000/cgi/slow_response.py")
	if (r.status_code == 504):
		print(GREEN, "Response timeout OK", RESET)
	else:
		print(RED, "Response timeout KO", RESET, f"status_code: {r.status_code}")

def connection_time_out(client_id):
	msg = 	"GET / HTTP/1.1\r\n" \
			"Host: localhost:3000\r\n" \
			"\r\n"

	HOST = "127.0.0.1"
	PORT = 3000
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST, PORT))
	s.send(msg.encode())
	data = s.recv(1024)
	time.sleep(20)
	if (s.recv(1024) == b''): # this ain't no good test
		print(GREEN, "Connection timeout OK", RESET)
	else:
		print(RED, "Connection timeout KO", RESET)


for i in range(n_clients):
	if i % 3 == 1:	
		x = threading.Thread(target=cgi_time_out, args=(i,))
	elif i % 3 == 2:	
		x = threading.Thread(target=incomple_request, args=(i,))
	else:
		x = threading.Thread(target=connection_time_out, args=(i,))
	x.start()
