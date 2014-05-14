AC_DEFUN([GEANY_EXTENSIONS],
[
	AC_ARG_ENABLE([extensions],
		[AS_HELP_STRING([--enable-extensions],
			[enable support for extensions/addons [default=auto]])],
		[geany_enable_extensions="$enableval"],
		[geany_enable_extensions="auto"])
	have_peas=no
	AS_IF([test "x$geany_enable_extensions" != "xno"],
	[
		PKG_CHECK_MODULES([PEAS], [libpeas-1.0 libpeas-gtk-1.0],
		[
			have_peas=yes
			AC_DEFINE([HAVE_PEAS], [1], [Define if being compiled with libpeas])
		], [have_peas=no])
		AS_IF([test "x$geany_enable_extensions" = "yes" -a "x$have_peas" = "no"],
		[
			AC_MSG_ERROR([Support for extensions was forced on but libpeas \
and its development files were not found, are they installed?])
		])
	])
	AM_CONDITIONAL([WITH_EXTENSIONS], [test "x$have_peas" = "xyes"])
	GEANY_STATUS_ADD([Enable extensions support], [$have_peas])
])
