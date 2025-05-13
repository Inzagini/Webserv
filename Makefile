CC:= c++
NAME:= webserv
CFLAGS:= -std=c++98 -Wall -Werror -Wextra -g
INCLUDES:= inc
SRC_DIR:= src
SRCS:= main.cpp
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

.PHONY: clean fclean re all
