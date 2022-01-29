SRC =	accept_connection.cpp \
		Fd_table.cpp \
		handle_requests.cpp \
		handle_responses.cpp \
		io.cpp \
		listening_socket.cpp \
		main.cpp \
		process_data.cpp \
		utils.cpp

HEADER =	includes.hpp Poll_array.hpp Fd_table.hpp

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
