#
# Makefile for Geany.app bundle.
#

INSTALL = install
GTK_MAC_BUNDLER = ~/.local/bin/gtk-mac-bundler
BUNDLE_FILES = geany.bundle geany.conf gtkrc launcher.sh Geany.icns Info-geany.plist
GTK_PREFIX = ~/gtk/inst

all: Geany.app

Geany.app: $(BUNDLE_FILES)
	$(GTK_MAC_BUNDLER) geany.bundle
	$(INSTALL) -m 0644 $(GTK_PREFIX)/lib/libiconv.2.dylib Geany.app/Contents/Resources/lib
	test -f $(GTK_PREFIX)/lib/libvte.9.dylib && $(INSTALL) -m 0644 $(GTK_PREFIX)/lib/libvte.9.dylib Geany.app/Contents/Resources/lib
	test -f $(GTK_PREFIX)/lib/libgtkmacintegration.0.dylib && $(INSTALL) -m 0644 $(GTK_PREFIX)/lib/libgtkmacintegration.0.dylib Geany.app/Contents/Resources/lib

clean:
	rm -rf Geany.app .Geany.app
