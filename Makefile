include Makefile.inc

#
# Settings
#

LIBNAME = libnmea.so

DESTDIR ?=

MACHINE=$(shell uname -m)
ifeq ($(strip $(MACHINE)),x86_64)
LIBDIR = usr/lib64
else
LIBDIR = usr/lib
endif
INCLUDEDIR = usr/include

MODULES = context generate generator gmath info parse parser sentence time tok util
OBJ = $(MODULES:%=build/%.o)

LIBS = -lm
INCS = -I ./include


#
# Targets
#

all: all-before lib/$(LIBNAME)

remake: clean all

lib/$(LIBNAME): $(OBJ)
	@echo "[LD] $@"
	@$(CC) -shared -Wl,-soname=$(LIBNAME) -o lib/$(LIBNAME) $(OBJ) -lc

build/%.o: src/%.c Makefile Makefile.inc
	@echo "[CC] $<"
	@$(CC) $(CCFLAGS) $(INCS) -c $< -o $@

samples: all
	$(MAKE) -C samples all


#
# Phony Targets
#

.PHONY: all-before clean doc install install-headers uninstall uninstall-headers

all-before:
	@mkdir -p build lib

clean:
	$(MAKE) -C doc clean
	$(MAKE) -C samples clean
	rm -fr build lib

doc:
	$(MAKE) -C doc all

install: all
	@mkdir -v -p $(DESTDIR)/$(LIBDIR)
	cp lib/$(LIBNAME) $(DESTDIR)/$(LIBDIR)/$(LIBNAME).$(VERSION)
	$(STRIP) $(DESTDIR)/$(LIBDIR)/$(LIBNAME).$(VERSION)
	ldconfig -n $(DESTDIR)/$(LIBDIR)

install-headers: all
	@mkdir -v -p $(DESTDIR)/$(INCLUDEDIR)
	@rm -fr $(DESTDIR)/$(INCLUDEDIR)/nmea
	cp -r include/nmea $(DESTDIR)/$(INCLUDEDIR)

uninstall:
	rm -f $(DESTDIR)/$(LIBDIR)/$(LIBNAME) $(DESTDIR)/$(LIBDIR)/$(LIBNAME).$(VERSION)
	ldconfig -n $(DESTDIR)/$(LIBDIR)
	@rmdir -v -p --ignore-fail-on-non-empty $(DESTDIR)/$(LIBDIR)

uninstall-headers:
	rm -fr $(DESTDIR)/$(INCLUDEDIR)/nmea
	@rmdir -v -p --ignore-fail-on-non-empty $(DESTDIR)/$(INCLUDEDIR)
