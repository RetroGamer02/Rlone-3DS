include ../../cfg/compilation.mk

.PHONY: clean
.MAIN: map.a

SOURCES!=ls *.c
OBJECTS=$(SOURCES:c=o)

.SUFFIXES: .c .o
.c.o:
	@echo "CC\t$@ $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

map.a: $(OBJECTS)
	@echo "AR\t$@ $(OBJECTS)"
	@$(AR) rcs $@ $(OBJECTS)

clean:
	@echo "RM\tmap.a $(OBJECTS)"
	rm -f map.a $(OBJECTS)
