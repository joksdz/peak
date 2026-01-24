# The name of your app
TARGET = peak

# The compiler
CC = gcc

# Compiler flags: -Wall for warnings, and pkg-config to find GTK headers
# # Update these lines in your Makefile
CFLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0 gtk-layer-shell-0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 gtk-layer-shell-0)
# The source files
SRC = dmenu.c

# The build rule
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean rule to remove the app
clean:
	rm -f $(TARGET)
