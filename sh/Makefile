# Only for the version.
include ../cfg/compilation.mk

SOURCES!=ls *.sh

all: ../$(BUILDDIR)/install.sh ../$(BUILDDIR)/depends.sh $(SOURCES)

../$(BUILDDIR)/install.sh: $(SOURCES)
	@echo "#!/usr/bin/env sh" > $@
	@echo "vsn=$(VSN)" >> $@
	@echo "depends_only=0" >> $@
	@echo "CAT\t$(BUILDDIR)/install.sh $(SOURCES)"
	@cat $(SOURCES) >> $@
	@chmod +x $@

../$(BUILDDIR)/depends.sh: $(SOURCES)
	@echo "#!/usr/bin/env sh" > $@
	@echo "vsn=$(VSN)" >> $@
	@echo "depends_only=1" >> $@
	@echo "CAT\t$(BUILDDIR)/install.sh $(SOURCES)"
	@cat $(SOURCES) >> $@
	@chmod +x $@
