if WITH_EXTENSIONS

extensionmoddir = $(libdir)/geany/extensions
extensionmod_LTLIBRARIES = libdemo.la
extensionmod_DATA = demo.plugin

libdemo_la_SOURCES = demo.c democfg.c
libdemo_la_CFLAGS = -I$(top_srcdir)/src $(PEAS_CFLAGS)
libdemo_la_LDFLAGS = -module -avoid-version
libdemo_la_LIBADD = $(PEAS_LIBS)

else

EXTRA_DIST = demo.c demo.plugin

endif
