CFLAGS:=-Wall -Werror -g
BUILDDIR:=build

all: client server
	
client: client.c measure.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $^

server: server.c measure.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $^

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
