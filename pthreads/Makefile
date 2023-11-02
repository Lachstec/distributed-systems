TARGET=Pthread
CC=clang
CFLAGS=-Wall -Werror -g -pthread

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm $(TARGET)
	rm -rf $(TARGET).dSYM
