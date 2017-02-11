TARGETS=$(patsubst %.c,%,$(wildcard *.c))

LDLIBS += -lpthread

all: $(TARGETS)

conn: occupy_portids.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

