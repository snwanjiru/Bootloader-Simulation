# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -g -I./src
TARGET = bootloader_sim

SRCS = src/main.c \
       src/bootloader.c \
       src/virtual_flash.c \
       src/protocol_uart.c \
       src/protocol_spi.c \
       src/protocol_i2c.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) uart

run-spi: $(TARGET)
	./$(TARGET) spi

run-i2c: $(TARGET)
	./$(TARGET) i2c

test: $(TARGET)
	./$(TARGET) uart

.PHONY: all clean run run-spi run-i2c test
