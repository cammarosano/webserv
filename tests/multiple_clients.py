import requests, threading, socket, time

RED = "\033[0;31m"
RESET = "\033[0m"
GREEN = "\033[0;32m"

urls = ["http://localhost:3000/",
		"http://localhost:3000/no_default_idx/",
		"http://localhost:3000/cgi/test.py"]

n_requests_per_client = 20
n_clients = 1000


# client tries to make n_requests in one connection, reconnects if necessary
def client_session(client_id):
	# print(f"Client {client_id} started")
	requests_sent = 0
	s = requests.Session()
	print(f"Client {client_id} connected.")
	while requests_sent < n_requests_per_client:
		if requests_sent % 4 == 3:
			r = s.post(urls[2], data={'hello':'you', 'bye':'me'})
		else:
			r = s.get(urls[requests_sent % 4])
		requests_sent += 1

	s.close()
	print(f"Client {client_id} finished. Requests sent: {requests_sent}")

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
	print(response_line)
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
	if (s.recv(1024) == b''):
		print(GREEN, "Connection timeout OK", RESET)
	else:
		print(RED, "Connection timeout KO", RESET)


for i in range(n_clients):
	if i % 13 == 0:	
		x = threading.Thread(target=cgi_time_out, args=(i,))
	elif i % 11 == 0:	
		x = threading.Thread(target=incomple_request, args=(i,))
	elif i % 17 == 0:	
		x = threading.Thread(target=connection_time_out, args=(i,))
	else:
		x = threading.Thread(target=client_session, args=(i,))
	x.start()
