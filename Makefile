NAME = ircserv
BOT_NAME = bot
CC = c++
CFLAGS = -I./includes -g3 -std=c++98
# CFLAGS+= -fsanitize=address
 CFLAGS += -Wall -Wextra -Werror
SRCDIR = src
OBJDIR = obj
BOT_OBJDIR = obj_bot
SRCS = $(filter-out $(wildcard $(SRCDIR)/*bot.cpp), $(wildcard $(SRCDIR)/*.cpp))
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

BONUS_SRCS = $(wildcard $(SRCDIR)/*bot.cpp)
BONUS_OBJS = $(BONUS_SRCS:$(SRCDIR)/%.cpp=$(BOT_OBJDIR)/%.o)

RM = rm -f
MKDIR = mkdir -p

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

bonus: $(BOT_NAME)

$(BOT_NAME): $(BONUS_OBJS)
	$(CC) $(CFLAGS) $(BONUS_OBJS) -o $(BOT_NAME)

b_run: fclean
	$(MAKE) -j20 bonus
	@clear
	./$(BOT_NAME) 127.0.0.1 6667 a

$(BOT_OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) $(BOT_OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJDIR) $(BOT_OBJDIR)

fclean: clean
	$(RM) $(NAME) $(BOT_NAME)

run: fclean
	$(MAKE) -j20 re
	@clear
	valgrind ./$(NAME) 6667 a

re: fclean all

.PHONY: all bonus clean fclean re
