SRC =	accept_connection.cpp \
		check4new_requests.cpp \
		io.cpp \
		listening_socket.cpp \
		main.cpp \
		setup.cpp \
		utils.cpp

CLASS = ARequestHandler.cpp \
		HttpRequest.cpp \
		StaticRH.cpp \
		ErrorRH.cpp \
		FdManager.cpp \
		ConfigParser.cpp \
		DirectoryRH.cpp \
		ACgiRH.cpp \
		RedirectRH.cpp \
		CgiGetRH.cpp \
		CgiPostRH.cpp \
		BodyDecoder.cpp

SRCS = $(addprefix src/,$(SRC)) $(addprefix src/classes/,$(CLASS))
HDR = includes.hpp FdManager.hpp ARequestHandler.hpp StaticRH.hpp \
		HttpRequest.hpp ErrorRH.hpp DirectoryRH.hpp ACgiRH.hpp RedirectRH.hpp \
		CgiGetRH.hpp CgiPostRH.hpp BodyDecoder.hpp
HEADERS = $(addprefix includes/,$(HDR))

OBJ = $(SRCS:.cpp=.o)
CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
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
