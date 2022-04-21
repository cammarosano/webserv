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


def parse_body() -> tuple[str, str, str]:
    boundary = ""
    file_name = ""
    for i, line in enumerate(sys.stdin):
        if i == 0:
            boundary = line
        if "Content-Disposition" in line:
            file_name = get_file_name(line)
        if line == "\r\n":
            break
    # reading file content
    line = sys.stdin.readline()
    content = ""
    while True:
        if line == '':
            break
        next = sys.stdin.readline()
        if boundary in next:
            eprint("found end")
            content += line
            break
        content += line
        line = next
    return boundary, file_name, content


method = os.environ["REQUEST_METHOD"]
content_type = os.environ["CONTENT_TYPE"]

boundary, file_name, content = parse_body()
# eprint(file_name)
eprint("boundary:", boundary)
eprint(content)

upload_dir = os.environ["UPLOAD_DIR"]
# with open("../" + upload_dir + "/" + file_name, "w") as f:
#     eprint("writing")

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
