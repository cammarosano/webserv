# shell cmd: find src/ -type f
SRC = \
functions/utils.cpp \
functions/new_requests.cpp \
functions/main.cpp \
functions/accept_connection.cpp \
functions/io.cpp \
functions/listening_socket.cpp \
functions/setup.cpp \
request_handlers/CgiGetRH.cpp \
request_handlers/ErrorRH.cpp \
request_handlers/StaticRH.cpp \
request_handlers/DirectoryRH.cpp \
request_handlers/ARequestHandler.cpp  \
request_handlers/ACgiRH.cpp \
request_handlers/CgiPostRH.cpp \
request_handlers/RedirectRH.cpp \
classes/FdManager.cpp \
classes/BodyDecoder.cpp \
classes/Client.cpp \
classes/HttpRequest.cpp \
classes/ConfigParser.cpp

# shell cmd: find includes/ -type f
INC = \
request_handlers/CgiGetRH.hpp \
request_handlers/ACgiRH.hpp \
request_handlers/StaticRH.hpp \
request_handlers/RedirectRH.hpp \
request_handlers/CgiPostRH.hpp \
request_handlers/DirectoryRH.hpp \
request_handlers/ErrorRH.hpp \
request_handlers/ARequestHandler.hpp \
classes/Client.hpp \
classes/HttpRequest.hpp \
classes/FdManager.hpp \
classes/ConfigParser.hpp \
classes/BodyDecoder.hpp \
macros.h \
utils.h \
config.hpp \
includes.hpp

HEADERS = $(addprefix includes/,$(INC)) 
OBJ = $(patsubst %.cpp, obj/%.o, $(SRC))
CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address
INCLUDES = -I includes -I includes/classes -I includes/request_handlers
NAME = webserv

all:	$(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $^ -o $@

obj/%.o:	src/%.cpp $(HEADERS) | obj/
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/:
			mkdir -p obj/request_handlers
			mkdir -p obj/classes
			mkdir -p obj/functions

clean:
			rm -f $(OBJ)
			rm -rf obj/

fclean:		clean
			rm -f $(NAME)

re:			clean all

.PHONY:		all clean fclean re
