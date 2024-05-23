CC = gcc

FILES = main.c core.c card.c sdl_funcs.c solitaire.c sleep.c

FLAGS = -std=gnu11 -O2

TARGET = solitaire

LIBS = -lSDL2

$(TARGET):
	$(CC) $(FLAGS) $(FILES) -o $(TARGET) $(LIBS)

all:
	$(TARGET)

clean:
	rm $(TARGET)