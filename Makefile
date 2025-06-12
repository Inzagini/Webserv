CXX:= c++
NAME:= webserv
CXXFLAGS:= -std=c++98 -g -Wall -Werror -Wextra
INCLUDES:= inc
SRC_DIR:= src
SRCS:= main.cpp cgi.cpp config.cpp parseRequest.cpp request.cpp response.cpp server.cpp utils.cpp
OBJS = $(SRCS:%.cpp=$(SRC_DIR)/%.o)
FILEPATH = ./conf.d/default.conf

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -I. -I$(INCLUDES) -o $(NAME)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDES) -c $< -o $@

clean:
	@rm -f $(OBJS) && echo "Cleaned *.o files"

fclean: clean
	@rm -f $(NAME) && echo "Cleaned $(NAME)"

re: fclean all

t: re
	./webserv $(FILEPATH)

.PHONY: clean fclean re all
