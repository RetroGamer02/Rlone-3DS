include ../cfg/compilation.mk

.PHONY: clean
.MAIN: rlone.a

SOURCES!=ls *.c
OBJECTS=$(SOURCES:c=o)

.SUFFIXES: .c .o
.c.o:
	@echo "CC\t$@ $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

rlone.a: $(OBJECTS)
	@echo "AR\t$@ $(OBJECTS)"
	@$(AR) rcs $@ $(OBJECTS)

clean:
	@echo "RM\trlone.a $(OBJECTS)"
	rm -f rlone.a $(OBJECTS)
