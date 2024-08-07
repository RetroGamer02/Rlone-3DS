# Overrideable values.
DEBUG?=NODEBUG
VSN?=1.02
CC?=cc
AR?=ar
O?=3

NAME=rlone

# SDL library header location. Sub-libs also use this directory typically.
CFLAGS_SDL!=pkg-config --cflags sdl2

CFLAGS_WARNS=-Wall -Wextra -Werror
CFLAGS_DEF=-D$(DEBUG) -DVSN=\"$(VSN)\" -g
CFLAGS_OPT=-O$(O)
CFLAGS_STD=-std=c99 -pedantic

_PWD!=pwd
BNAME!=pwd | xargs basename
CFLAGS_INC?=-I$(_PWD)/include -I$(_PWD)/iniparser/src

# Put it all together.
CFLAGS=$(CFLAGS_WARNS) $(CFLAGS_OPT) $(CFLAGS_SDL) $(CFLAGS_INC) $(CFLAGS_STD) $(CFLAGS_DEF)
