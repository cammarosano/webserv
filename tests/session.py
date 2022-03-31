import requests

with requests.Session() as s:
	while (True):
		r = s.get("http://localhost:3000/no_hay")
		print(r.status_code)