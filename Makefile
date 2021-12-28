NAME = webserver

CXX =		clang++
CXXFLAGS =	-Wall -Wextra -Werror -std=c++98

SRC_FILES =	main.cpp server.cpp socket.cpp Response.cpp Request.cpp

SRCS = $(SRC_FILES)
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(OBJS) $(CXXFLAGS) -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all