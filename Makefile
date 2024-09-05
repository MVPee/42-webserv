NAME = webserv

OBJS_DIR = .objs
SRCS = 	main.cpp \
		srcs/Server.cpp \
		srcs/Request.cpp \
		srcs/Response.cpp \
		srcs/Config.cpp \
		srcs/Post.cpp \
		srcs/Get.cpp

OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:%.cpp=%.o))

FLAGS = -std=c++98 -g -fsanitize=address #-Wall -Werror -Wextra

RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@c++ $(FLAGS) $(OBJS) -o $(NAME)
	@echo "\n${RED}./$(NAME)\n${NC}"

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@c++ $(FLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJS_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

run: all
	./$(NAME) server.conf