SRC =	main.cpp\
		Webserv/Webserv.cpp\
		Webserv/Addrinfo.cpp\
		Webserv/Fsocket.cpp\
		Webserv/VirtualServ.cpp\
		Webserv/ServerBlock.cpp\
		Webserv/Route.cpp\
		Webserv/lib_function.cpp\
		Http_protocol/Post.cpp\
		Http_protocol/Get.cpp\
		Http_protocol/Delete.cpp\
		Http_protocol/Http_header.cpp\
		Http_protocol/req_function.cpp\
		Http_protocol/Request.cpp\
		Http_protocol/Error.cpp\
		parse_config.cpp\
		Exception.cpp\

OBJ = $(SRC:%.cpp=%.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

NAME = server

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) -c $^ -o $@ $(CFLAGS)

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean
	make all

.PHONY : all clean fclean re