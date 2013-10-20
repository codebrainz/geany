dnl GEANY_CHECK_GLIB_MKENUMS
dnl Checks for glib-mkenums utility and fails if not found.
AC_DEFUN([GEANY_CHECK_GLIB_MKENUMS], [
	AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums], [no])
	AS_IF([test "x$GLIB_MKENUMS" = "xno"], [
		AC_MSG_ERROR([Unable to locate 'glib-mkenums' utility, is it installed?])
	])
])
