#!/usr/bin/env bash

########################################################################
# Build script for Geany 32 or 64-bit using MSYS2                      #
# -------------------------------------------------------------------- #
# Dependencies:                                                        #
#   - First you need to install MSYS2 and NSIS. Depending on where     #
#     they installed, you may need to edit the `MSYS2_PREFIX' and      #
#     `NSIS_PREFIX' variables below. If you installed them in their    #
#     default locations there should be no need to modify those        #
#     variables below (assuming 64-bit build machine).                 #
#   - If you plan to produce 32-bit builds, run the following command  #
#     in the MSYS2 shell:                                              #
#       $ pacman -S mingw32/mingw-w64-i686-toolchain                   #
#   - For a 64-bit builds, use:                                        #
#       $ pacman -S mingw64/mingw-w64-x86_64-toolchain                 #
#   - Open the appropriate MSYS2 terminal depending on the             #
#     architecture. For 32-bit builds, use the `mingw32_shell.bat'     #
#     script in the MSYS2 installation directory. For 64-bit builds,   #
#     use the `mingw64_shell.bat' script to start the MSYS2 terminal   #
#     emulator.                                                        #
#                                                                      #
# Usage:                                                               #
#   $ mkdir ~/geany-build                                              #
#   $ cd ~/geany-build                                                 #
#   $ /path/to/geanysrc/scripts/win32-installer-msys2.sh [ARCH]        #
#   < lots of output and a long wait >                                 #
#                                                                      #
# Notes:                                                               #
#   - If no architecture is specified, a 32-bit installer will be      #
#     generated. To build a 64-bit installer, run the script with the  #
#     `x86_64' argument.                                               #
#   - All generated files will be put below the current working        #
#     directory.                                                       #
#   - You can specify additional options passed to GNU Make using the  #
#     `MAKEFLAGS' environment variable. This is especially useful if   #
#     you have multiple CPUs or CPU cores, you can pass the `-j' flag  #
#     to Make to tell it how many parallel processes to launch.        #
########################################################################

set -e


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Customizable section                                                 #

# Adjust this to wherever NSIS is installed, this is the default on
# recent Windows versions.
NSIS_PREFIX="/c/Program Files (x86)/NSIS"

# Adjust this to wherever msys2 is installed, usually `/c/msys64' or
# `/c/msys32', depending on your OS architecture and which version
# you installed.
MSYS2_PREFIX="/c/msys64"

# End of customizable section, shouldn't need to edit anything else.   #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#


#
# Setup global variables based on arguments and working dir
#
if [ ! -z "$1" ]
then
	if [ "$1" = "i686" -o "$1" = "x86" ]
	then
		BITS="32"
		TARGET_NAME="i686"
	elif [ "$1" = "x86_64" -o "$1" = "amd64" ]
	then
		BITS="64"
		TARGET_NAME="x86_64"
	fi
else
	BITS="32"
	TARGET_NAME="i686"
fi
MINGW="mingw$BITS"
ARCH="mingw-w64-${TARGET_NAME}"
TARGET="${TARGET_NAME}-w64-mingw32"
WORKDIR=`pwd`
SRCDIR=$(realpath $(dirname $(dirname "${BASH_SOURCE[0]}")))
BUILDDIR="$WORKDIR/$TARGET_NAME/build"
PREFIX="$WORKDIR/$TARGET_NAME/prefix"
STAGEDIR="$WORKDIR/$TARGET_NAME/stage"
GENSTAMP="$WORKDIR/autogen.stamp"
CONFSTAMP="$WORKDIR/autoconf.stamp"

#
# Ensures the needed packages are installed and creates working dirs.
#
function setup_environment()
{
	pacman -Sy # sync
	pacman --noconfirm --needed -S \
		msys/autoconf \
		msys/automake \
		msys/intltool \
		msys/make \
		$MINGW/$ARCH-docutils \
		$MINGW/$ARCH-doxygen \
		$MINGW/$ARCH-gtk3 \
		$MINGW/$ARCH-libtool \
		$MINGW/$ARCH-toolchain
	mkdir -p "$PREFIX" "$BUILDDIR" "$STAGEDIR"
}

#
# Generates the build system files
#
function generate_build_system()
{
	if [ ! -f "$GENSTAMP" -o ! -f "$SRCDIR/configure" ]
	then
		pushd "$SRCDIR"
		NOCONFIGURE=1 ./autogen.sh \
			&& echo "Delete this file to cause 'autogen.sh' script to be re-run" \
				> "$GENSTAMP"
		popd
	fi
}

#
# Configures the build system by calling `configure' script.
#
function configure_build_system()
{
	if [ ! -f "$CONFSTAMP" ]
	then
		pushd "$BUILDDIR"
		CPPFLAGS="-DGEANY_WIN32_INSTALLER" \
			"$SRCDIR/configure" \
				--prefix="$PREFIX" \
				--target="$TARGET" \
				--enable-gtk3 \
					&& echo "Delete the file to cause 'configure' script to be re-run" \
						> "$CONFSTAMP"
		popd
	fi
}

#
# Changes to the appropriate branch (if specified) and ensures the
# build system files are generated.
#
function prepare_source_code()
{
	# if $1 is set, checkout that branch in source tree
	if [ ! -z "$1" ]
	then
		pushd "$SRCDIR"
		if [ "$1" != "master" ]
		then
			git checkout -b build_$1 $1
		else
			git checkout master
		fi
		popd
	fi
	generate_build_system
	configure_build_system
}

#
# Compiles source code and installs into `$PREFIX'.
#
function compile_source_code()
{
	prepare_source_code $1
	pushd "$BUILDDIR"
		make $MAKEFLAGS install
	popd
}

#
# Takes the needed files from GTK3 Adwaita theme
#
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

	mkdir -p "$STAGEDIR/Icons/Adwaita"
	cp -v index.theme "$STAGEDIR/Icons/Adwaita"

	for size in $sizes
	do
		if [ -d "$size" ]
		then
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
		fi
	done

	popd
}

#
# Moves all files included in the release into the `$STAGEDIR'.
#
function stage_files()
{
	# Geany files
	pushd "$STAGEDIR"
	mkdir -p Data Documentation Icons Include Plugins Locales
	popd

	pushd "$PREFIX"
	cp -v bin/* "$STAGEDIR/"
	cp -rv data/* "$STAGEDIR/Data/"
	pushd share/doc/geany
		cp -v AUTHORS "$STAGEDIR/Documentation/Authors.txt"
		cp -v COPYING "$STAGEDIR/Documentation/License.txt"
		cp -v manual.txt "$STAGEDIR/Documentation/Manual.txt"
		cp -v NEWS "$STAGEDIR/Documentation/News.txt"
		cp -v README "$STAGEDIR/Documentation/Readme.txt"
		cp -v ScintillaLicense.txt "$STAGEDIR/Documentation/"
		cp -v THANKS "$STAGEDIR/Documentation/Thanks.txt"
		cp -v TODO "$STAGEDIR/Documentation/Todo.txt"
	popd
	cp -rv share/icons/* "$STAGEDIR/Icons/"
	cp -rv include/geany/* "$STAGEDIR/Include/"
	cp -v lib/geany/*.dll "$STAGEDIR/Plugins/"
	cp -rv share/locale/* "$STAGEDIR/Locales/"
	popd

	# Fixup line-endings on Documentation text files so Notepad can view
	pushd "$STAGEDIR/Documentation"
	for FILE in `ls *.txt`
	do
		sed -i 's/$/\r/' "$FILE"
	done
	popd

	# Files from MSYS2
	pushd "$MSYS2_PREFIX/$MINGW"
	if [ "$MINGW" = "mingw64" ]
	then
		cp -v bin/libgcc_s_seh-1.dll $STAGEDIR
	else
		cp -v bin/libgcc_s_dw2-1.dll $STAGEDIR
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
		"$STAGEDIR"
	popd

	# Icons
	build_icon_theme
}

#
# Generates the NSIS installer and compiles it with `makensis.exe'.
#
function generate_installer()
{
	pushd "$WORKDIR"
	# convert msys2-style paths to windows-style paths
	local stage_dir=$(echo "$STAGEDIR" | sed -e 's/\/c\//C:\\\\/g' -e 's/\//\\\\/g')
	local src_dir=$(echo "$SRCDIR" | sed -e 's/\/c\//C:\\\\/g' -e 's/\//\\\\/g')
	local work_dir=$(echo "$WORKDIR" | sed -e 's/\/c\//C:\\\\/g' -e 's/\//\\\\/g')
	# replace some variables in the NSIS script
	sed \
		-e "s|@@bits@@|$BITS|" \
		-e "s|@@arch@@|$TARGET_NAME|" \
		-e "s|@@stagedir@@|$stage_dir|" \
		-e "s|@@srcdir@@|$src_dir|" \
		-e "s|@@workdir@@|$work_dir|" \
			"$SRCDIR/scripts/win32-installer-msys2.nsi.in" \
				> "$BUILDDIR/win32-installer-msys2.nsi"
	"$NSIS_PREFIX/makensis.exe" "$BUILDDIR/win32-installer-msys2.nsi"
	popd
}

#
# Main entry-point, comment-out steps if desired.
#
function main()
{
	setup_environment
	compile_source_code
	stage_files
	generate_installer
}
main $@
