NAME =  server
CXX = c++ 

SRC = $(wildcard *.cpp) \

OBJ = $(SRC:.cpp=.o)

LUMO_LIB = Lumo/liblumo.a

all: $(NAME)

$(NAME): $(LUMO_LIB) $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LUMO_LIB) -lssl -lcrypto -lcurl

$(LUMO_LIB):
	$(MAKE) -j$(nproc) -C Lumo

clean:
	rm -rf $(OBJ)

fclean: clean
	make fclean -C Lumo
	rm -rf $(NAME)

re: clean all

.PHONY: all clean fclean re