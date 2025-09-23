NAME = ircserv
CC = c++
CFLAGS = -std=c++98 -I./includes -g3
CFLAGS+= -fsanitize=address
CFLAGS += -Wall -Wextra -Werror
SRCS = $(wildcard src/*.cpp)
RM = rm -f

all: $(NAME)

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(NAME)

clean:
	@echo "Nothing to clean (no .o files used)"

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
