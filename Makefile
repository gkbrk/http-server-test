CC := cc
CFLAGS := -Ofast -Wall -Werror -Wextra -std=c11 -pedantic -Isrc -pthread
CFLAGS += -flto -s
#CFLAGS += -ggdb
#CFLAGS += -pg -g
#CFLAGS += -g

C_FILES   := $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJ_FILES := $(patsubst src/%.c, bin/%.o, $(C_FILES))

bin/main: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o "$@" $^

bin/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o "$@" "$<"

run: bin/main
	@"$<"
.PHONY: run

clean:
	@rm -rf bin
.PHONY: clean
