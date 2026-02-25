NAME =  liblumo.a

CXX = c++
CPPFLAGS = -Wall -Werror -Wextra

SRC = $(wildcard *.cpp) \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: clean all

.PHONY: all clean fclean re