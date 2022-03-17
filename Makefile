SRC =	accept_connection.cpp \
		check4new_requests.cpp \
		io.cpp \
		listening_socket.cpp \
		main.cpp \
		setup.cpp \
		utils.cpp \
		ARequestHandler.cpp \
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

HDR = includes.hpp FdManager.hpp ARequestHandler.hpp StaticRH.hpp \
		HttpRequest.hpp ErrorRH.hpp DirectoryRH.hpp ACgiRH.hpp RedirectRH.hpp \
		CgiGetRH.hpp CgiPostRH.hpp BodyDecoder.hpp

HEADERS = $(addprefix includes/,$(HDR))

OBJ = $(patsubst %.cpp, obj/%.o, $(SRC))
CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
INCLUDES = -I includes
NAME = webserv

all:	$(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $^ -o $@

obj/%.o:	src/%.cpp $(HEADERS) | obj/
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/:
			mkdir -p obj/

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f $(NAME)

re:			clean all

.PHONY:		all clean fclean re
