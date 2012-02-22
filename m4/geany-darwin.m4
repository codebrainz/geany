dnl GEANY_CHECK_DARWIN
dnl Checks whether we're building for Mac OSX, and defines appropriate stuff
dnl if it is the case.
dnl Most importantly, AM_CODITIONALs DARWIN
AC_DEFUN([GEANY_CHECK_DARWIN],
[
	case "${host}" in
		*darwin*)
			AC_DEFINE([GEANY_DARWIN], [1], [Define to 1 for Mac/Darwin builds])
			AM_CONDITIONAL([DARWIN], true)
			;;
		*)
			AM_CONDITIONAL([DARWIN], false)
			;;
	esac
])
