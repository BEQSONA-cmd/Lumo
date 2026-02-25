NAME =  server
CXX = c++
CPPFLAGS = -Wall -Werror -Wextra 

SRC = $(wildcard *.cpp) \

OBJ = $(SRC:.cpp=.o)

LUMO_LIB = Lumo/liblumo.a

all: $(NAME)

$(NAME): $(OBJ)
	make -C Lumo
	$(CXX) $(CPPFLAGS) $(OBJ) -o $(NAME) $(LUMO_LIB) -lssl -lcrypto

clean:
	rm -rf $(OBJ)

fclean: clean
	make fclean -C Lumo
	rm -rf $(NAME)

re: clean all

.PHONY: all clean fclean re