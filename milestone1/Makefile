BUILDDIR:=build
SRCS:= $(shell find . -name '*.c')
PROGS:=$(patsubst %.c,%,$(SRCS))
CFLAGS:=-Wall -Werror -g

%: %.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $<
	
all: $(PROGS)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) 


