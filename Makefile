CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -O0 -ggdb -no-pie
LIBS = -lm

SRC_DIR = src
SRCS = $(SRC_DIR)/cachesim.c \
$(SRC_DIR)/cache.c \
$(SRC_DIR)/arguments.c \
$(SRC_DIR)/txt_reader.c \
$(SRC_DIR)/metrics.c

OBJS = $(SRCS:.c=.o)
TARGET = cachesim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

