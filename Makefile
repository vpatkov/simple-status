TARGET = simple-status
PREFIX = /usr/local

SOURCES = $(wildcard *.c modules/*.c)

CC = gcc

CFLAGS = -c -Wall -std=c17 -Ofast -flto -funsigned-bitfields
CFLAGS += -Wall -Wextra -Wundef -Wlogical-op -Wredundant-decls -Wshadow
CFLAGS += -MMD -MP
CFLAGS += -I.

LDFLAGS = -s -Ofast -flto -lasound -lX11

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(TARGET).o $(SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

-include $(SOURCES:.c=.d)

clean:
	rm -f $(TARGET) *.o *.d modules/*.o modules/*.d

install: $(TARGET)
	install -m 755 $(TARGET) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
