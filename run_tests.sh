#!/bin/bash

RED="\033[0;31m"
RESET="\033[0m"
GREEN="\033[0;32m"

# siege 25 users
siege -b -f tests/urls4siege -t10s

# siege 255 users - GET request only
siege -b http://localhost:3000 -t10s -c 255

# test Request timeout
echo -e $GREEN"Sending incomplete HTTP request header"$RESET
python3 tests/incomplete_header.py 

# test Response timeout
echo -e $GREEN"GET CGI slow script"$RESET
curl http://localhost:3000/cgi/slow_response.py

# multiple clients, some time-outs expected
echo -e $GREEN"A bit of everything..."$RESET
python3 tests/multiple_clients.py 
