
NAME = webserv

CC = clang++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98 -fsanitize=address

# DEFINES = -D OUT=0# -D TIME=1

ifeq ($(shell uname -s),Darwin)
OS = mac
else
OS = linux
endif

PWD = $(shell pwd)

INCLUDES = -I includes

CLASSES = -I srcs/autoindex -I srcs/cgi -I srcs/config -I srcs/request -I srcs/response -I srcs/server

HEADER = webserv.hpp

MAIN = webserv

AUTOINDEX = AutoIndexGenerator

CGI = CgiHandler

CONFIG = Config ConfigReader ConfigServer RequestConfig

REQUEST = Request

RESPONSE = Response ResponseHeader

SERVER = Server Cluster

TOOLS = utility

SRC = $(addsuffix .cpp, $(MAIN)) \
	$(addprefix autoindex/, $(addsuffix .cpp, $(AUTOINDEX))) \
	$(addprefix cgi/, $(addsuffix .cpp, $(CGI))) \
	$(addprefix config/, $(addsuffix .cpp, $(CONFIG))) \
	$(addprefix request/, $(addsuffix .cpp, $(REQUEST))) \
	$(addprefix response/, $(addsuffix .cpp, $(RESPONSE))) \
	$(addprefix server/, $(addsuffix .cpp, $(SERVER))) \
	$(addprefix utils/, $(addsuffix .cpp, $(TOOLS))) \

SRC_DIR = srcs

OBJ = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(MAIN))) \
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(AUTOINDEX))) \
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(CGI))) \
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(CONFIG))) \
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(REQUEST))) \
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(RESPONSE))) \
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SERVER))) \
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(TOOLS))) \

OBJ_DIR = objs

OBJ_SUBDIR = $(addprefix $(OBJ_DIR)/, autoindex cgi config request response server socket utils)

OBJ_BUILD = $(addprefix $(OBJ_DIR)/, $(SRC:cpp=o))

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ_BUILD)
	@$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $(CLASSES) -o $(NAME) $(OBJ_BUILD)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $(CLASSES) -o $@ -c $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_SUBDIR)

clean:
	@rm -rf $(OBJ_DIR) $(OBJ_SUBDIR)

fclean: clean
	@rm -f $(NAME)
re: fclean all

test: test_$(OS)

test_mac:
	@osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear"'
	@osascript -e 'tell application "Terminal" to activate'
	./webserv www/conf/webserv.conf

test_linux:
	@x-terminal-emulator --working-directory=$$(pwd) -x ./webserv www/conf/webserv.conf

check_mac: all
	@osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear && time ./test_mac/macos_tester http://localhost:8000"'
	@osascript -e 'tell application "Terminal" to activate'
	./webserv test_mac/mac.conf

check_linux: all
	@x-terminal-emulator --working-directory=$$(pwd) -x "time ./test_linux/ubuntu_tester http://localhost:8000"
	./webserv test_linux/linux.conf

.PHONY: clean fclean re test test_setup test_mac test_linux check_mac check_linux
