dnl GEANY_CHECK_DARWIN
dnl Checks whether we're building for Mac OSX, and defines appropriate stuff
dnl if it is the case.
dnl Most importantly, AM_CODITIONALs DARWIN
AC_DEFUN([GEANY_CHECK_DARWIN],
[
	case "${host}" in
		*darwin*)
			AC_PROG_OBJC
			AM_PROG_CC_C_O
			AC_DEFINE([GEANY_DARWIN], [1], [Define to 1 for Mac/Darwin builds])
			AM_CONDITIONAL([DARWIN], true)
			have_gtk_mac_integration=yes
			PKG_CHECK_MODULES([GTK_MAC], [gtk-mac-integration], 
				[have_gtk_mac_integration=false], 
				[have_gtk_mac_integration=true])
			if test $have_gtk_mac_integration = xyes
			then
				AC_DEFINE([MAC_INTEGRATION], [1], [Define to 1 for gtk-mac-integration support])
			fi
			;;
		*)
			AM_CONDITIONAL([DARWIN], false)
			;;
	esac
])
