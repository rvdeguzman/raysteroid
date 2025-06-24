CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INCLUDES = -Iinclude
LIBS = -Llib -lraylib
TARGET = game
SOURCE = src/main.c

# macOS specific flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Linux specific flags
ifeq ($(UNAME_S),Linux)
    LIBS += -lGL -lm -lpthread -ldl -lrt -lX11
endif

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCE) $(LIBS)

run: 
	make
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
