import sys

data = sys.stdin.buffer.read()
idx = data.find(b'\r\n\r\n')
# print(idx)
string = data[:idx].decode()
print(string)
data = data[idx + 4:]
idx = data.find(b'--DELIMITER--')
data = data[:idx]

with open("uploaded_file.png", 'wb') as file:
    file.write(data)
