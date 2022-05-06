#ifndef MACROS_H
#define MACROS_H

#define BUFFER_SIZE 8192
#define QUEUE_MAX_SIZE 1000
#define MAX_CLIENTS 500
#define CONN_CLOSE_THRESHOLD 0.8
#define REQUEST_TIMEOUT 5	  // seconds
#define RESPONSE_TIMEOUT 30	  // seconds
#define CONNECTION_TIMEOUT 60 // seconds
#define MIN_IDLE_TIME 5		  // seconds
#define POLL_TIMEOUT 2000	  // miliseconds
#define DEFAULT_CONFIG_FILE "conf/default.conf"
#define MIME_TYPES_FILE "conf/mime.types"
#define DEFAULT_MIME "text/plain"

#endif