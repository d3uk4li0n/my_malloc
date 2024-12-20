CC=gcc
CFLAGS+=-Wall -Wextra -Werror 
OBJ+=src/utility.o src/main.o
TARGET+=my_malloc

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

.PHONY: all clean fclean all
clean:
	rm -f src/*.o

fclean: clean
	rm -f $(TARGET)

re: fclean all
