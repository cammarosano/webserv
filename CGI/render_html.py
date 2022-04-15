import os, sys, requests

method = os.environ["REQUEST_METHOD"]

if method == "POST":
	input_str = sys.stdin.read()
else:
	input_str = os.environ["QUERY_STRING"]

# decode url encoded string
input_str = input_str.replace('+', ' ')
input_str = requests.utils.unquote(input_str)

# build a vars dict
vars_lst = input_str.split('&')
vars_dict = {}
for kv in vars_lst:
	pair = kv.split('=')
	vars_dict[pair[0]] = pair[1]

# get template
f = open("template.html", 'r')
template_str = f.read()
f.close()

# render body
try:
	body = template_str.format(**vars_dict)
except:
	body = "HTML page could not be rendered"

# output response header
# print("HTTP/1.1 200 OK")
print("Content-Type: text/html; charset=UTF-8", end="\r\n")
print("Content-Length: " + str(len(body)), end="\r\n\r\n")

# output response body
print(body, end='')
