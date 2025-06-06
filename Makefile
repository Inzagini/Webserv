CC:= c++
NAME:= webserv
CFLAGS:= -std=c++98  -g
INCLUDES:= inc
SRC_DIR:= src
SRCS:= main.cpp cgi.cpp config.cpp parseRequest.cpp request.cpp response.cpp server.cpp TO_DELETE.cpp
OBJS = $(SRCS:%.cpp=$(SRC_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -I. -I$(INCLUDES) -o $(NAME)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@

clean:
	@rm -f $(OBJS) && echo "Cleaned *.o files"

fclean: clean
	@rm -f $(NAME) && echo "Cleaned $(NAME)"

re: fclean all

t:
	./webserv

.PHONY: clean fclean re all
