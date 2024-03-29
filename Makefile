# shell cmd: find src/ -type f
SRC = \
functions/utils.cpp \
functions/new_requests.cpp \
functions/main.cpp \
functions/accept_connection.cpp \
functions/io.cpp \
functions/listening_socket.cpp \
functions/setup.cpp \
functions/handle_requests.cpp \
functions/clear_resources.cpp \
functions/error_resp_no_request.cpp \
functions/mime_type.cpp \
functions/init_response.cpp \
functions/house_keeper.cpp \
request_handlers/CgiGetRH.cpp \
request_handlers/ErrorRH.cpp \
request_handlers/StaticRH.cpp \
request_handlers/DirectoryRH.cpp \
request_handlers/ARequestHandler.cpp  \
request_handlers/ACgiRH.cpp \
request_handlers/CgiPostRH.cpp \
request_handlers/RedirectRH.cpp \
request_handlers/PostRH.cpp \
request_handlers/DeleteRH.cpp \
classes/FdManager.cpp \
classes/BodyDecoder.cpp \
classes/Client.cpp \
classes/HttpRequest.cpp \
classes/ConfigParser.cpp \
classes/config.cpp \
classes/HttpResponse.cpp

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
request_handlers/DeleteRH.hpp \
request_handlers/PostRH.hpp \
classes/Client.hpp \
classes/HttpRequest.hpp \
classes/FdManager.hpp \
classes/ConfigParser.hpp \
classes/BodyDecoder.hpp \
classes/HttpResponse.hpp \
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

valgrind:	CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
valgrind:	clean $(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $^ -o $@

obj/%.o:	src/%.cpp $(HEADERS) | obj
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj:
			mkdir -p obj/request_handlers
			mkdir -p obj/classes
			mkdir -p obj/functions

clean:
			rm -f $(OBJ)
			rm -rf obj

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
