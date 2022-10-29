TARGET = simple-status
PREFIX = /usr/local

SOURCES = $(wildcard *.c)

CC = gcc

CFLAGS = -c -Wall -std=c17 -Ofast -flto -funsigned-bitfields
CFLAGS += -Wall -Wextra -Wundef -Wlogical-op -Wredundant-decls -Wshadow
CFLAGS += -MMD -MP

LDFLAGS = -s -Ofast -flto
#LDFLAGS += -lX11 -lcurl -lbsd -lm -lasound

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(TARGET).o $(SOURCES:.c=.o)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

-include $(SOURCES:.c=.d)

clean:
	rm -f $(TARGET) *.o *.d

install: $(TARGET)
	install -m 755 $(TARGET) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
