#!/bin/bash

P=/Users/gtkdev/gtk/inst
B=/Users/gtkdev/Projects/gtk
L=/Users/gtkdev/Projects/gtk/Geany/geany/osx/Geany.app/Contents/Resources/
OPT="${1}"

# rebuild Murrine GTK+ engine
function rebuild_murrine 
{
    echo "******** Rebuilding Murrine engine..."
    cd "${B}/Murrine/"
    make clean
    ./configure --prefix=$P && \
    make V=1 && \
    make install
    cd "$B"
}

# rebuild libVTE
function rebuild_vte
{
    echo "******** Rebuilding VTE library..."
    cd "${B}/Vte/"
    make clean
    ./configure --prefix=$P --disable-Bsymbolic && \
    make V=1 && \
    make install
    cd ..
}

# rebuild Geany
function rebuild_geany 
{
    echo "******** Rebuilding Geany..."
    cd "${B}/Geany/geany"
    make clean
    ./autogen.sh --prefix=$P && \
    make V=1 && \
    make install
    cd "$B"
}

# recreate .app bundle for Geany
function rebuild_bundle
{
    echo "******** Rebuilding Geany .app bundle..."
    cd "${B}/Geany/geany/osx/"
    rm -r Geany.app
    rm -rf .Geany.app
    "${HOME}/.local/bin/gtk-mac-bundler" geany.bundle
    cp "${P}/lib/libiconv.2.dylib" \
		"${P}/lib/libvte.9.dylib" \
        "${L}/lib"
    cd "$B"
}

case "$OPT" in 
    "murrine")
        rebuild_murrine;
        ;;
    "vte")
        rebuild_vte;
        ;;
    "geany")
        rebuild_geany;
        ;;
    "bundle")
        rebuild_bundle;
        ;;
    "all")
        rebuild_murrine;
        rebuild_vte;
        rebuild_geany;
        rebuild_bundle;
        ;;
    *)
        echo "Usage: $0 [all|murrine|vte|geany|bundle]";
        exit 0;
        ;;
esac

echo "Rebuild complete"
