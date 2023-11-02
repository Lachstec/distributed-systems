TARGET=HelloWorld
CC=clang
CFLAGS=-Wall -Werror -g

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm $(TARGET)
	rm -rf $(TARGET).dSYM
