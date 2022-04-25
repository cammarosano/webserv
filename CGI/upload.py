import os
import sys


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def get_file_name(body: str) -> str:
    file_name = body[body.find("filename"):]
    file_name = file_name[file_name.find("=") + 1:file_name.find("\n")]
    file_name = file_name.strip("\"")[:-2]
    return file_name


method = os.environ["REQUEST_METHOD"]
content_type = os.environ["CONTENT_TYPE"]
upload_dir = os.environ["UPLOAD_DIR"]
boundary = content_type[content_type.find("boundary=") + 9:]

# eprint(file_name)
body = sys.stdin.read()

COLORS = {
    "GREEN": '\033[92m',
    "RESET": '\033[0m',
    "CYAN": '\033[96m'
}

file_name = get_file_name(body)

boundary = boundary.strip()
begin = body.find("\r\n\r\n") + 4
end = boundary.find("\r\n--" + boundary + "--")
file_content = body[begin:end]
file_content = file_content[:file_content.find("\r\n--" + boundary + "--")]

# remove space at the end
# file content parsed
with open("../" + upload_dir + "/" + file_name, "wb") as f:
    #for some reason write fails when uploading non text files
    f.write(file_content)
response_body = "<!DOCTYPE html>"
response_body += "<html>"
response_body += "<head>"
response_body += "<title> CGI Script output </title>"
response_body += "</head>"
response_body += "<body>"
response_body += "<p>This is a CGI script to upload files </p>"
response_body += f'<h4>{upload_dir} has been created</h4>'
response_body += "</body>"
response_body += "</html>"
print("Content-Type: text/html; charset=UTF-8")
print("Content-Length: " + str(len(response_body)), end="\r\n\r\n")

# output response body
print(response_body, end='')
