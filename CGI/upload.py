import os
import sys
import io


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def get_file_name(body: str) -> str:
    file_name = body[body.find("filename"):]
    file_name = file_name[file_name.find("=") + 1:file_name.find("\n")]
    file_name = file_name.strip("\"")[:-2]
    return file_name


def get_file_content(body: str, boundary: str) -> str:
    res = body.split('\n')
    # res = [elem.strip() for elem in res if len(elem.strip()) > 0]
    for i, elem in enumerate(res):
        if "Content-Type" in elem:
            content = res[i + 1:-1]
            eprint(content)
    return ""


method = os.environ["REQUEST_METHOD"]
content_type = os.environ["CONTENT_TYPE"]
boundary = content_type[content_type.find("boundary=") + 9:]

if method == "POST":
    body = sys.stdin.read()
else:
    body = os.environ["QUERY_STRING"]

COLORS = {
    "GREEN": '\033[92m',
    "RESET": '\033[0m',
    "CYAN": '\033[96m'
}

# content = get_file_content(body, boundary)
file_name = get_file_name(body)

boundary = boundary.strip()
begin = body.find("\r\n\r\n") + 4
end = boundary.find("\r\n--" + boundary + "--\r\n")
file_content = body[begin:end]
file_content = file_content[:file_content.find("\r\n--" + boundary + "--")]
file_content = file_content[:file_content.find("\r\n")]
with open(file_name, "w") as f:
    f.write(file_content)
response_body = "<!DOCTYPE html>"
response_body += "<html>"
response_body += "<head>"
response_body += "<title> CGI Script output </title>"
response_body += "</head>"
response_body += "<body>"
response_body += "<p>This is a CGI script to upload files </p>"
response_body += f'<h4>{file_name} has been created</h4>'
response_body += "</body>"
response_body += "</html>"
print("Content-Type: text/html; charset=UTF-8")
print("Content-Length: " + str(len(response_body)), end="\r\n\r\n")

# output response body
print(response_body, end='')
