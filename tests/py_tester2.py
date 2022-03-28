import requests

with requests.Session() as s:
	for _ in range(100):
		r = s.get("http://localhost:3000/")
		r = s.get("http://localhost:3000/form.html")
		r = s.get("http://localhost:3000/cgi/test.py")
		r = s.post("http://localhost:3000/cgi/test.py", data={'hello':'you', 'bye':'me'} )
