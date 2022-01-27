SRC =	main.cpp \
		listening_socket.cpp \
		accept_connection.cpp \
		handle_incoming_data.cpp \
		handle_outbound_data.cpp \
		handle_get_request.cpp \
		utils.cpp

HEADER =	includes.hpp Poll_array.hpp

OBJ = $(SRC:.cpp=.o)
CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
NAME = webserv

all:	$(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $^ -o $@

%.o:		%.cpp $(HEADER)
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f $(NAME)

re:			clean all

.PHONY:		all clean fclean re
