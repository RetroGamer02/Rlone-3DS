include ./cfg/compilation.mk
include ./cfg/libraries.mk

BUILDDIR?=build
DISTDIR?=dist
OS=$(shell uname)
ARCH=$(shell arch)
DISTOS=$(shell echo $(OS) | tr '[:upper:]' '[:lower:]')

COMPONENTS=src/rlone.a src/renderer/renderer.a src/map/map.a src/object/object.a iniparser/libiniparser.a

.PHONY: clean dump secondary run dist $(COMPONENTS) $(BUILDDIR)/installer
.MAIN: $(NAME)

# Forward user-defined values.
FORWARDS=DEBUG=$(DEBUG) O=$(O) CC=$(CC) AR=$(AR) CFLAGS_INC="$(CFLAGS_INC)"

$(NAME): dump secondary $(COMPONENTS)
	@echo "CC\t$@ $(COMPONENTS)"
	@$(CC) -o $(BUILDDIR)/$@ $(COMPONENTS) $(CFLAGS) $(LIBS)

dump:
	@echo "BUILD VARIABLES"
	@echo "       OS: $(OS)"
	@echo "     ARCH: $(ARCH)"
	@echo "   CFLAGS: $(CFLAGS)"
	@echo "     LIBS: $(LIBS)"
	@echo "BUILD DIR: $(BUILDDIR)"
	@echo " DIST DIR: $(DISTDIR)"
	@echo

secondary: 
	@echo "MK\t$(BUILDDIR)/ dist/"
	@mkdir -p $(BUILDDIR)/ dist/
	@echo "CP\tres/ $(BUILDDIR)/res/"
	@rm -rf $(BUILDDIR)/res
	@cp -r res $(BUILDDIR)/res
	@echo "CP\tLICENSE $(BUILDDIR)/LICENSE"
	@echo "Rlone\n=====" > $(BUILDDIR)/SOURCE.LICENSES
	@cat LICENSE >> $(BUILDDIR)/SOURCE.LICENSES
	@echo "\nlibiniparser\n============" >> $(BUILDDIR)/SOURCE.LICENSES
	@cat iniparser/LICENSE >> $(BUILDDIR)/SOURCE.LICENSES
	@echo "CP\tREADME $(BUILDDIR)/README"
	@cp README $(BUILDDIR)/

iniparser/libiniparser.a:
	@echo "MAKE\tiniparser/"
	@make -sC iniparser/

src/rlone.a:
	@echo "MAKE\t$@"
	@make -sC src/ $(FORWARDS)

src/renderer/renderer.a:
	@echo "MAKE\t$@"
	@make -sC src/renderer/ $(FORWARDS)

src/map/map.a:
	@echo "MAKE\t$@"
	@make -sC src/map/ $(FORWARDS)

src/object/object.a:
	@echo "MAKE\t$@"
	@make -sC src/object/ $(FORWARDS)

installer: $(NAME)
	@make -sC sh/ $(FORWARDS) BUILDDIR=$(BUILDDIR) all

install: $(NAME) installer
	@cd build/ && sh install.sh

run: $(NAME)
	@echo "RUN\n"
	@cd build/ && ./rlone

dist: $(NAME) installer
	@echo "STRIP\t$(BUILDDIR)/rlone"
	@strip "$(BUILDDIR)/rlone"
	@echo "TAR\tbuild/"
	@mv $(BUILDDIR) rlone && zip -r $(DISTDIR)/rlone-$(DISTOS).zip rlone/ && mv rlone $(BUILDDIR)
	@echo "TAR\t/"
	@cd ../ && zip -r $(BNAME)/$(DISTDIR)/rlone-source.zip $(BNAME)/ -x \*.o \*.a $(BNAME)/dist/\* $(BNAME)/build/\* $(BNAME)/vita/build/\* $(BNAME)/.git/\* \*.swp $(BNAME)/iniparser/libiniparser.so.1

clean:
	@make -sC iniparser/ clean
	@make -sC src/renderer/ clean
	@make -sC src/object/ clean
	@make -sC src/map/ clean
	@make -sC src/ clean
	@echo "RM\tbuild/ dist/"
	@rm -rf build/ dist/
