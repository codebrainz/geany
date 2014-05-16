
PREFIX=/opt/geany
GISCAN ?= g-ir-scanner
GICOMP ?= g-ir-compiler
TYPELIB_DIR ?= $(PREFIX)/lib/girepository-1.0/
GIR_DIR ?= $(PREFIX)/share/gir-1.0/

all: Geany-1.0.gir Geany-1.0.typelib

Geany-1.0.gir: ../../src/*.[ch]
	$(GISCAN) --pkg=geany --program=../../build/src/geany -I../../src \
		--no-libtool --include=Gtk-3.0 --include=Peas-1.0 \
		--namespace=Geany --nsversion=1.0 \
		--output=$@ $^

Geany-1.0.typelib: Geany-1.0.gir
	$(GICOMP) --output=$@ $^

make install: Geany-1.0.gir Geany-1.0.typelib pydemo.plugin
	mkdir -p $(GIR_DIR)
	mkdir -p $(TYPELIB_DIR)
	cp Geany-1.0.gir $(GIR_DIR)
	cp Geany-1.0.typelib $(TYPELIB_DIR)
	cp pydemo.plugin pydemo.py $(PREFIX)/lib/geany/extensions/

clean:
	$(RM) Geany-1.0.gir Geany-1.0.typelib
