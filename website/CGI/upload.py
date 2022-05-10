import os
import sys
import io


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def get_file_name(body: bytes) -> str:
    file_name = body[body.find(b"filename"):]
    file_name = file_name[file_name.find(b"=") + 1:file_name.find(b"\n")]
    file_name = file_name.strip(b"\"")[:-2]
    return file_name.decode()


method = os.environ["REQUEST_METHOD"]
content_type = os.environ["CONTENT_TYPE"]
upload_dir = os.environ["UPLOAD_DIR"]
boundary = content_type[content_type.find("boundary=") + 9:]

body = sys.stdin.buffer.read()

file_name = get_file_name(body)

boundary = boundary.strip()
begin = body.find(b"\r\n\r\n") + 4
end = body.find(b"\r\n--" + boundary.encode() + b"--")
file_content = body[begin:end]
file_content = file_content[:file_content.find(
    b"\r\n--" + boundary.encode() + b"--")]

with open("../" + upload_dir + "/" + file_name, "wb") as f:
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
