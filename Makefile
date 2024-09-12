NAME := webserv

RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m

OBJS_DIR := .objs
SOURCES = main.cpp \
			srcs/Server.cpp \
			srcs/Request.cpp \
			srcs/Response.cpp \
			srcs/Config.cpp \
			srcs/Client.cpp \
			srcs/Cookie.cpp \
			srcs/methods/Post.cpp \
			srcs/methods/Get.cpp \
			srcs/methods/Cgi.cpp

OBJECTS := $(patsubst %.cpp,$(OBJS_DIR)/%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,$(OBJS_DIR)/%.d,$(SOURCES))

CXX := g++
CXXFLAGS := -std=c++98 -g -fsanitize=address #-Wall -Werror -Wextra

# Default rule
all: $(NAME)

# Link the executable
$(NAME): $(OBJECTS)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "\n${RED}./$(NAME)\n${NC}"

# Include dependency files
-include $(DEPENDS)

# Rule to compile .cpp files into .o files
$(OBJS_DIR)/%.o: %.cpp Makefile
	@mkdir -p $(@D)  # Create directory if it doesn't exist
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Clean rules
clean:
	@$(RM) -rf $(OBJS_DIR)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

run: all
	./$(NAME)

.PHONY: all clean fclean re run
