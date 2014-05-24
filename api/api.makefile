VALAC ?= valac
VFLAGS = $(VALAC_FLAGS) --pkg gtk+-3.0 --vapidir . --pkg geany_ \
	--Xcc="-I../src" --Xcc="-I../tagmanager/src" --Xcc="--include=geany.h" \
	--Xcc="-I../scintilla/include" --save-temps \
	--header=geanyapi.h --use-header \
	--gir=Geany-1.0.gir --library=geany-1.0

INT_VAPI = geany_.vapi
SOURCES = document.vala tab.vala window.vala view.vala

all: libgeany-1.0.so

libgeany-1.0.so: $(SOURCES) $(INT_VAPI)
	$(VALAC) $(VFLAGS) -o $@ $(SOURCES)

clean:
	$(RM) $(SOURCES:.vala=.c) geanyapi.h
