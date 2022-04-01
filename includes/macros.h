#ifndef MACROS_H
# define MACROS_H

# define BUFFER_SIZE 8192
# define QUEUE_MAX_SIZE 1000
# define MAX_CLIENTS 500
# define CONN_CLOSE_THRESHOLD 0.5
# define REQUEST_TIME_OUT 5 // seconds
# define RESPONSE_TIME_OUT 5 // seconds
# define CONNECTION_TIME_OUT 60 // seconds LOW VALUE FOR TESTING
# define MIN_IDLE_TIME 5 // seconds
# define REAPER_FREQUENCY 5 // seconds
# define POLL_TIME_OUT 2000 // miliseconds
# define DEFAULT_MIME "text/plain"
# define DEFAULT_CONFIG_FILE "conf/default.conf"
# define MIME_TYPES_FILE "conf/mime.types"
# ifndef DEBUG
	# define DEBUG 0
# endif

// debug stuff
# define RED "\033[0;31m"
# define RESET "\033[0m"
# define GREEN "\033[0;32m"
# define COUTDEBUG(x, c) std::cout << c << x << RESET << std::endl

#endif