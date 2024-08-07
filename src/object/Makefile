include ../../cfg/compilation.mk

.PHONY: clean
.MAIN: object.a

SOURCES!=ls *.c
OBJECTS=$(SOURCES:c=o)

.SUFFIXES: .c .o
.c.o:
	@echo "CC\t$@ $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

object.a: $(OBJECTS)
	@echo "AR\t$@ $(OBJECTS)"
	@$(AR) rcs $@ $(OBJECTS)

clean:
	@echo "RM\tobject.a $(OBJECTS)"
	rm -f object.a $(OBJECTS)
