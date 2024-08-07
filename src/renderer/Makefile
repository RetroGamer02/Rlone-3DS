include ../../cfg/compilation.mk

.PHONY: clean
.MAIN: renderer.a

SOURCES!=ls *.c
OBJECTS=$(SOURCES:c=o)

.SUFFIXES: .c .o
.c.o:
	@echo "CC\t$@ $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

renderer.a: $(OBJECTS)
	@echo "AR\t$@ $(OBJECTS)"
	@$(AR) rcs $@ $(OBJECTS)

clean:
	@echo "RM\trenderer.a $(OBJECTS)"
	rm -f renderer.a $(OBJECTS)
