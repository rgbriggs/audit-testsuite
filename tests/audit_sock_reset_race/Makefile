TARGETS=$(patsubst %.c,%,$(wildcard *.c))

LDLIBS += -lpthread

all: $(TARGETS)

socket_fuzz: socket_fuzz.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
