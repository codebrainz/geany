#
# Just a GNU Makefile to avoid having to integrate Vala into the build
# system for a demo plugin.
#

VALAC ?= valac
VD_PREFIX = /opt/geany
VD_PLUG_DIR = $(VD_PREFIX)/lib/geany/extensions
VD_PCFILE = $(VD_PREFIX)/lib/pkgconfig/geany.pc
VD_CFLAGS = $(shell pkg-config --cflags $(VD_PCFILE) libpeas-1.0) -I. -I../../src
VD_LIBS = $(shell pkg-config --libs $(VD_PCFILE) libpeas-1.0)
VD_VALAFLAGS = --pkg gtk+-3.0 --pkg libpeas-1.0 --vapidir=. --pkg geany

all: libvalademo.so

libvalademo.so: valademo.o
	$(CC) -shared $(VD_CFLAGS) -o $@ $^ $(VD_LIBS)

valademo.o: valademo.c
	$(CC) -c -fPIC $(VD_CFLAGS) -o $@ $<

valademo.c: valademo.vala
	$(VALAC) -C $^ $(VD_VALAFLAGS)

install: libvalademo.so valademo.plugin
	cp $^ $(VD_PLUG_DIR)

clean:
	$(RM) -f valademo.c *.so

.PHONY: all clean
