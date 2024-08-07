# Puts all the dependencies into a nice "LIBS" variable.
LIBS_SDL!=pkg-config --libs sdl2
LIBS_IMG!=pkg-config --libs SDL2_image
LIBS_MIX!=pkg-config --libs SDL2_mixer
LIBS_INI=-liniparser
LIBS_M=-lm

LIB_DIRS=-Liniparser/

LIBS=$(LIB_DIRS) $(LIBS_SDL) $(LIBS_IMG) $(LIBS_MIX) $(LIBS_INI) $(LIBS_M)
