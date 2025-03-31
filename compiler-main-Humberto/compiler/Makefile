CC = gcc
CFLAGS = -I./ -g
OBJECTS = build/compiler.o build/cprocess.o build/lexer.o build/lex_process.o \
          build/helpers/buffer.o build/helpers/vector.o

all: compiler

compiler: $(OBJECTS)
	$(CC) main.c $(OBJECTS) $(CFLAGS) -o compiler

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf build compiler

.PHONY: all clean