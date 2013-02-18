#
# Makefile for libc.hiken
#

.PHONY: all clean

PLATFORM = LIBCHIKEN_FOR_LINUX

SRC_PATH = lib
INCLUDE_PATH = include

GLOBAL_GCC_OPTS = -g -std=gnu99 -I$(INCLUDE_PATH) -D$(PLATFORM) -DNDEBUG \
				  -fno-builtin -Wall -Wextra -pedantic

include $(SRC_PATH)/string/make.inc
include $(SRC_PATH)/stdio/make.inc
include $(SRC_PATH)/stdlib/make.inc

all: libc.a

libc.a: $(LIBC_OBJ)
	ar -rcs $@ $+

$(SRC_PATH)/%.o: $(SRC_PATH)/%.c
	gcc -c -I$(INCLUDE_PATH) $(GLOBAL_GCC_OPTS) -o $@ $<

clean:
	rm -rf $(LIBC_OBJ) libc.a

