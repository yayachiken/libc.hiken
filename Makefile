#
# Makefile for libc.hiken
#

.PHONY: all clean

SRC_PATH = lib
INCLUDE_PATH = include

GLOBAL_GCC_OPTS = -std=gnu99 -Wall -Wextra -pedantic

LIBC_OBJ = $(SRC_PATH)/string.o

all: libc.a

libc.a: $(LIBC_OBJ)
	ar -a $@

$(SRC_PATH)/%.o: $(SRC_PATH)/%.c
	gcc -c -I$(INCLUDE_PATH) $(GLOBAL_GCC_OPTS) -o $@ $<

clean:
	rm -rf $(SRC_PATH)/*.o libc.a

