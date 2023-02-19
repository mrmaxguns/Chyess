CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -pedantic
CURSES := -lncursesw

SRC_DIR := src
BUILD_DIR := build
INSTALL_DIR := /usr/local/bin

.PHONY: all
all: $(BUILD_DIR)/chyess

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: install
install: $(BUILD_DIR)/chyess
	mv -f $(BUILD_DIR)/chyess $(INSTALL_DIR)

.PHONY: uninstall
uninstall:
	rm -f $(INSTALL_DIR)/chyess

$(BUILD_DIR)/chyess: $(BUILD_DIR)/main.o $(BUILD_DIR)/board.o $(BUILD_DIR)/gamelogic.o $(BUILD_DIR)/ai.o
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/chyess $(BUILD_DIR)/main.o $(BUILD_DIR)/board.o \
	    $(BUILD_DIR)/gamelogic.o $(BUILD_DIR)/ai.o $(CURSES)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/main.o -c $(SRC_DIR)/main.c

$(BUILD_DIR)/board.o: $(SRC_DIR)/board.c $(SRC_DIR)/board.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/board.o -c $(SRC_DIR)/board.c

$(BUILD_DIR)/gamelogic.o: $(SRC_DIR)/gamelogic.c $(SRC_DIR)/gamelogic.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/gamelogic.o -c $(SRC_DIR)/gamelogic.c

$(BUILD_DIR)/ai.o: $(SRC_DIR)/ai.c $(SRC_DIR)/ai.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/ai.o -c $(SRC_DIR)/ai.c
