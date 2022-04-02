import requests, threading, socket, time

urls = ["http://localhost:3000/",
		"http://localhost:3000/no_default_idx/",
		"http://localhost:3000/cgi/test.py"]

n_requests_per_client = 300

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


# driver code

client_session(0)
