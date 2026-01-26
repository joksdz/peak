PREFIX = /usr/local
SHARE_DIR = $(PREFIX)/share/peak

CFLAGS += -D CSS_INSTALL_PATH=\"$(SHARE_DIR)/style.css\"

all: peak

peak: dmenu.c
	gcc $(CFLAGS) dmenu.c -o peak `pkg-config --cflags --libs gtk+-3.0 gtk-layer-shell-0`

# The Install Step
install: all
	mkdir -p $(SHARE_DIR)
	cp style.css $(SHARE_DIR)/
	cp peak $(PREFIX)/bin/

uninstall:
	rm -rf $(SHARE_DIR)
	rm $(PREFIX)/bin/peak
