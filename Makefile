# Configurações básicas
CC = gcc
CFLAGS = -I./ -g
BUILD_DIR = ./build
HELPERS_DIR = ./helpers

# Objetos principais
MAIN_OBJS = $(BUILD_DIR)/compiler.o \
            $(BUILD_DIR)/cprocess.o \
            $(BUILD_DIR)/lex_process.o \
            $(BUILD_DIR)/lexer.o \
			$(BUILD_DIR)/node.o \
			$(BUILD_DIR)/parser.o \
            $(BUILD_DIR)/$(HELPERS_DIR)/buffer.o \
            $(BUILD_DIR)/$(HELPERS_DIR)/vector.o

# Regras principais
.PHONY: all clean test test_lexer

all: main

main: $(MAIN_OBJS) main.c
	mkdir -p $(BUILD_DIR)
	$(CC) main.c $(CFLAGS) $(MAIN_OBJS) -o ./main

# Regras para objetos
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(HELPERS_DIR)/%.o: $(HELPERS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ./main ./tests/*.o
	rm -rf $(BUILD_DIR)