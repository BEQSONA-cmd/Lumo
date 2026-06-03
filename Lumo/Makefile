NAME =  liblumo.a

CXX = c++
CPPFLAGS = -Wall -Werror -Wextra

SRC = $(wildcard *.cpp) \
	  $(wildcard **/*.cpp)

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(OBJ): | nlohmann

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: clean all

nlohmann:
	git clone https://github.com/BEQSONA-cmd/nlohmann-json-hpp-file.git nlohmann \

.PHONY: all clean fclean re