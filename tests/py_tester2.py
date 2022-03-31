import requests, threading

urls = ["http://localhost:3000/",
		"http://localhost:3000/form.html",
		"http://localhost:3000/cgi/test.py"]

n_requests_per_client = 20
n_clients = 300


def client(client_id):
	print(f"Client {client_id} started")
	requests_sent = 0
	s = requests.Session()
	while (requests_sent < n_requests_per_client):
		try:
			while requests_sent < n_requests_per_client:
				if requests_sent % 4 == 3:
					r = s.post(urls[2], data={'hello':'you', 'bye':'me'})
				else:
					r = s.get(urls[requests_sent % 4])
				requests_sent += 1
		except requests.exceptions.RequestException as err:
			print(f"Exception at client {client_id}: {err}")
			s.close()
			s = requests.Session()

	print(f"Client {client_id} finished. Requests sent: {requests_sent}")

for client_id in range(n_clients):
	x = threading.Thread(target=client, args=(client_id,))
	x.start()
