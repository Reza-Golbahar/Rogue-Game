CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lncurses

SRCS = main.c game.c users.c menu.c
OBJS = $(SRCS:.c=.o)
TARGET = game

$(TARGET): $(OBJS)
    $(CC) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f $(OBJS) $(TARGET)

run: $(TARGET)
    ./$(TARGET)