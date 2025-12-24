TARGET = os_lab_2

CC = gcc

# Флаги компиляции:
# -Wall -Wextra : выводят максимум предупреждений о потенциальных ошибках
# -g            : добавляет отладочную информацию (для gdb)
# -pthread      : критически важен для работы с потоками (POSIX threads)
CFLAGS = -Wall -Wextra -g -pthread

SRCS = $(wildcard *.c)

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: all
	./$(TARGET)

.PHONY: all clean run
