
	
HEADERS = blk_connection.h blk_message.h blk_parser.h common.h server.h simple_buffer.h deps/logger/logger.h connection_table.h
OBJECTS = blk_connection.o blk_message.o blk_parser.o server.o simple_buffer.o deps/logger/logger.o connection_table.o

TARGET=main

BIN_DIR=$(HOME)/BIN

MAIN_OBJECTS=main.o $(OBJECTS)

all: $(TARGET)

test: $(TEST_TARGETS)

install: all
	cp $(TARGET)  $(BIN_DIR)

uninstall:
	rm $(BIN_DIR)/$(TARGET)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(main.o)
	-rm -f $(TARGET)
	-rm -f $(TEST_TARGETS)	
	
%.o: %.c $(HEADERS)
	gcc -c $<  -I/Users/rob/MyCurrentProjects/Pixie/pixie-uv/pixie-uv/pixie-uv/deps  -o $@

$(TARGET): $(MAIN_OBJECTS)
	gcc $(MAIN_OBJECTS) -o $@ -I/Users/rob/MyCurrentProjects/Pixie/pixie-uv/pixie-uv/pixie-uv/deps -luv

