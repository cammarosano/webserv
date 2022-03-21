#ifndef MACROS_H
# define MACROS_H

# define BUFFER_SIZE 4096
# define QUEUE_MAX_SIZE 10
# define REQUEST_TIME_OUT 5 // seconds
# define POLL_TIME_OUT 2000 // miliseconds
# define DEBUG 0

// debug stuff
# define RED "\033[0;31m"
# define RESET "\033[0m"
# define GREEN "\033[0;32m"
# define COUTDEBUG(x, c) std::cout << c << x << RESET << std::endl

#endif