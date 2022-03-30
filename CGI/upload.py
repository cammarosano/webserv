import os, sys, requests

method = os.environ["REQUEST_METHOD"]
body = ''
for line in sys.stdin:
	body += line + '<br>'

# output response header
print("HTTP/1.1 200 OK")
print("Content-Type: text/html; charset=UTF-8")
print("Content-Length: " + str(len(body)), end="\r\n\r\n")

# output response body
print(body, end='')

