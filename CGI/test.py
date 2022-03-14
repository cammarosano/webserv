import os, sys

method = os.environ["REQUEST_METHOD"]

# receive body if POST request
if method == "POST":
	body = sys.stdin.read()

response_body = "<!DOCTYPE html>"
response_body += "<html>"
response_body += "<head>"
response_body += "<title> CGI Script output </title>"
response_body += "</head>"
response_body += "<body>"
response_body += "<p>This is a CGI script! Here's the env: </p>"
response_body += "<ul>"
for k,v in os.environ.items():
	response_body += f'<li>{k}={v}</li>'
response_body += "</ul>"
if method == "POST":
	response_body += "<p>And here's the request's body:</p>"
	response_body += "<p>" + body + "</p>"
response_body += "</body>"
response_body += "</html>"

# output response header
print("HTTP/1.1 200 OK")
print("Content-Type: text/html; charset=UTF-8")
print("Content-Length: " + str(len(response_body)), end="\r\n\r\n")
# print("Content-Length: " + str(len(response_body)))
# print()

# output response body
print(response_body, end='')
