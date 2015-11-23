#!/usr/bin/env bash

set -e

WORKDIR=`pwd`
BUILDDIR=$WORKDIR/build
PREFIX=$WORKDIR/prefix
STAGEDIR=$WORKDIR/stage
SRCDIR=$(realpath $(dirname $(dirname "${BASH_SOURCE[0]}")))

# Adjust this to wherever NSIS is installed
NSIS_PREFIX="/c/Program Files (x86)/NSIS"

# Adjust this to wherever msys2 is installed (usually /c/msys64)
MSYS2_PREFIX=/c/tmp-msys64

# Adjust this based on 32-bit or 64-bit build
MINGW="mingw64"
ARCH="mingw-w64-x86_64"
TARGET="x64_64-w64-mingw32"
#MINGW="mingw32"
#ARCH="mingw-w64-i686"
#TARGET="i686-w64-mingw32"
REPO=https://github.com/geany/geany.git

function setup_environment()
{
	pacman -Sy # sync
	pacman --noconfirm --needed -S \
		msys/autoconf \
		msys/automake \
		msys/pkgconfig \
		msys/libtool \
		msys/intltool \
		msys/gettext \
		msys/glib2-devel \
		msys/make \
		msys/wget \
		msys/unzip \
		$MINGW/$ARCH-gcc \
		$MINGW/$ARCH-glib2 \
		$MINGW/$ARCH-gtk3 \
		$MINGW/$ARCH-toolchain
	mkdir -p "$PREFIX" "$BUILDDIR" "$STAGEDIR"
}

function configure_source_code()
{
	pushd "$BUILDDIR"
	PKG_CONFIG_PATH=$PCDIR \
	CPPFLAGS="-DGEANY_WIN32_INSTALLER" \
		"$SRCDIR/configure" \
			--prefix="$PREFIX" \
			--target="$TARGET" \
			--enable-gtk3 \
			--disable-html-docs \
			--disable-api-docs
	popd
}

function get_source_code()
{
	pushd "$WORKDIR"
	if [ ! -d "$SRCDIR" ]
	then
		git clone $REPO
	fi
	pushd "$SRCDIR"
	NOCONFIGURE=1 ./autogen.sh
	popd
	configure_source_code
	if [ ! -z "$1" ]
	then
		if [ "$1" != "master" ]
		then
			git checkout -b build_$1 $1
		else
			git checkout master
		fi
	fi
	popd
}

function compile_source_code()
{
	get_source_code $1
	pushd $BUILDDIR
	make $MAKEFLAGS install
	popd
}

function build_icon_theme()
{
	pushd "$MSYS2_PREFIX/$MINGW/share/icons/Adwaita"

	local sizes="8x8 16x16 22x22 24x24 32x32 48x48 64x64 96x96 256x256 scalable"
	local icons="\
		actions/application-exit.png \
		actions/document-new.png \
		actions/document-open.png \
		actions/document-open-recent.png \
		actions/document-save.png \
		actions/document-revert.png \
		actions/document-properties.png \
		actions/edit-clear.png \
		actions/edit-find.png \
		actions/go-previous.png \
		actions/go-next.png \
		actions/go-jump.png \
		actions/list-add-symbolic.symbolic.png \
		actions/list-remove-symbolic.symbolic.png \
		actions/pan-down-symbolic.symbolic.png \
		actions/pan-end-symbolic.symbolic.png \
		actions/system-run.png \
		actions/window-close.png \
	"
	#	status/image-missing.png \

	mkdir -p "$STAGEDIR/Icons/Adwaita"
	cp -v index.theme "$STAGEDIR/Icons/Adwaita"

	for size in $sizes
	do
		pushd "$size"
		for icon in $icons
		do
			local category=$(basename $(dirname "$icon"))
			local icon_dir="$STAGEDIR/Icons/Adwaita/$size/$category"
			if [ -f "$icon" ]
			then
				if [ ! -d "$icon_dir" ]
				then
					mkdir -p "$icon_dir"
				fi
				cp -v "$icon" "$icon_dir"
			fi
		done
		popd
	done

	popd
}

function stage_files()
{
# Geany files
	pushd $STAGEDIR
	mkdir -p Data Documentation Icons Include Plugins Locales
	popd

	pushd $PREFIX
	cp -v bin/* $STAGEDIR/
	cp -rv data/* $STAGEDIR/Data/
	pushd share/doc/geany
		cp -v AUTHORS $STAGEDIR/Documentation/Authors.txt
		cp -v COPYING $STAGEDIR/Documentation/License.txt
		cp -v manual.txt $STAGEDIR/Documentation/Manual.txt
		cp -v NEWS $STAGEDIR/Documentation/News.txt
		cp -v README $STAGEDIR/Documentation/Readme.txt
		cp -v ScintillaLicense.txt $STAGEDIR/Documentation/
		cp -v THANKS $STAGEDIR/Documentation/Thanks.txt
		cp -v TODO $STAGEDIR/Documentation/Todo.txt
	popd
	cp -rv share/icons/* $STAGEDIR/Icons/
	cp -rv include/geany/* $STAGEDIR/Include/
	cp -v lib/geany/*.dll $STAGEDIR/Plugins/
	cp -rv share/locale/* $STAGEDIR/Locales/
	popd

# Fixup line-endings on Documentation text files so Notepad can view
	pushd $STAGEDIR/Documentation
	for FILE in `ls *.txt`
	do
		sed -i 's/$/\r/' "$FILE"
	done
	popd

# Files from MSYS2
	pushd $MSYS2_PREFIX/$MINGW
	if [ "$MINGW" = "mingw64" ]
	then
		cp -v bin/libgcc_s_seh-1.dll $STAGEDIR
	else
		cp -v bin/libgcc_s_dw2.dll $STAGEDIR
	fi
	cp -v \
		bin/libstdc++-6.dll \
		bin/libcairo-2.dll \
		bin/libgdk-3-0.dll \
		bin/libgdk_pixbuf-2.0-0.dll \
		bin/libgio-2.0-0.dll \
		bin/libglib-2.0-0.dll \
		bin/libgmodule-2.0-0.dll \
		bin/libgobject-2.0-0.dll \
		bin/libgtk-3-0.dll \
		bin/libintl-8.dll \
		bin/libpango-1.0-0.dll \
		bin/libpangocairo-1.0-0.dll \
		bin/libwinpthread-1.dll \
		bin/libcairo-gobject-2.dll \
		bin/libepoxy-0.dll \
		bin/libpng16-16.dll \
		bin/libfontconfig-1.dll \
		bin/libfreetype-6.dll \
		bin/libpixman-1-0.dll \
		bin/libffi-6.dll \
		bin/libiconv-2.dll \
		bin/libpangoft2-1.0-0.dll \
		bin/libpangowin32-1.0-0.dll \
		bin/libexpat-1.dll \
		bin/libbz2-1.dll \
		bin/libharfbuzz-0.dll \
		bin/libatk-1.0-0.dll \
		bin/zlib1.dll \
		$STAGEDIR
	#cp -rv share/icons/Adwaita $STAGEDIR/Icons/
	popd

	build_icon_theme
}

function generate_installer()
{
	local bits="64"
	local arch="x86_64"
	case "$ARCH" in
		*x86_64*)
			bits="64"
			arch="x86_64"
			;;
		*i686*)
			bits="32"
			arch="i686"
			;;
	esac

	pushd "$WORKDIR"
	# convert msys2-style paths to windows-style paths
	local stage_dir=$(echo "$STAGEDIR" | sed -e 's/\/c\//C:\\\\/g' -e 's/\//\\\\/g')
	local src_dir=$(echo "$SRCDIR" | sed -e 's/\/c\//C:\\\\/g' -e 's/\//\\\\/g')
	# replace some variables in the NSIS script
	sed \
		-e "s|@@bits@@|$bits|" \
		-e "s|@@arch@@|$arch|" \
		-e "s|@@stagedir@@|$stage_dir|" \
		-e "s|@@srcdir@@|$src_dir|" \
			"$SRCDIR/scripts/win32-installer-msys2.nsi.in" \
				> "$BUILDDIR/win32-installer-msys2.nsi"
	"$NSIS_PREFIX/makensis.exe" "$BUILDDIR/win32-installer-msys2.nsi"
	popd
}

#
# run the various steps
#

#setup_environment
#compile_source_code
#stage_files
generate_installer
