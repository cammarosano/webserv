import requests, threading

def client(client_id):
	print(f"Client {client_id} started")
	with requests.Session() as s:
		for _ in range(100):
			r = s.get("http://localhost:3000/")
			r = s.get("http://localhost:3000/form.html")
			r = s.get("http://localhost:3000/cgi/test.py")
			r = s.post("http://localhost:3000/cgi/test.py", data={'hello':'you', 'bye':'me'} )
	print(f"Client {client_id} finished")

for client_id in range(20):
	x = threading.Thread(target=client, args=(client_id,))
	x.start()
