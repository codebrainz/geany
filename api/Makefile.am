lib_LTLIBRARIES = libgeany-1.0.la

libgeany_1_0_la_CFLAGS = \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/tagmanager/src \
	-I$(top_srcdir)/scintilla/include \
	--include=geany.h

libgeany_1_0_la_LIBADD = $(GTK_LIBS)

libgeany_1_0_la_VALAFLAGS = \
	--pkg gtk+-3.0 \
	--vapidir=. --pkg geany_ \
	--header=geanyapi.h --use-header \
	--library=geany-1.0 \
	--gir=Geany-1.0.gir \
	--vapi=geany-1.0.vapi \
	--Xcc="-I$(top_srcdir)/src" \
	--Xcc="-I$(top_srcdir)/tagmanager/src" \
	--Xcc="-I$(top_srcdir)/scintilla/include"

libgeany_1_0_la_SOURCES = \
	document.vala \
	filetype.vala \
	view.vala \
	window.vala

girdir = $(datadir)/gir-1.0
gir_DATA = Geany-1.0.gir
Geany-1.0.gir: libgeany-1.0.la

vapidir = $(datadir)/vala/vapi
vapi_DATA = geany-1.0.vapi geany-1.0.deps
geany-1.0.vapi: libgeany-1.0.la

apidir = $(includedir)/geany-1.0/geany
api_HEADERS = geany.h geanyapi.h
geanyapi.h: libgeany-1.0.la

EXTRA_DIST = geany_.vapi
