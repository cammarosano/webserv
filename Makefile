SRC =	accept_connection.cpp \
		ARequestHandler.cpp \
		check4new_requests.cpp \
		ErrorRH.cpp \
		FdManager.cpp \
		io.cpp \
		listening_socket.cpp \
		main.cpp \
		HttpRequest.cpp \
		setup.cpp \
		StaticRH.cpp \
		utils.cpp \
		ConfigParser.cpp \

HDR = includes.hpp FdManager.hpp ARequestHandler.hpp StaticRH.hpp \
		HttpRequest.hpp ErrorRH.hpp
HEADERS = $(addprefix includes/,$(HDR))

OBJ = $(SRC:.cpp=.o)
CC = clang++
CFLAGS = #-Wall -Wextra -Werror -std=c++98 -g
INCLUDES = -I includes
NAME = webserv

all:	$(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $^ -o $@

%.o:		%.cpp $(HEADERS)
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f $(NAME)

re:			clean all

.PHONY:		all clean fclean re
