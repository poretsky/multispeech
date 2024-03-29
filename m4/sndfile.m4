# Check and configure paths for libsndfile
#
# Designed especially for the Multispeech project
# by Igor B. Poretsky <poretsky@mlbox.ru>
#
#	Adds the following arguments to configure:
# --with-sndfile-prefix=DIR
#
# A user of libsndfile should add @SNDFILE_LIBS@ and
# @SNDFILE_CPPFLAGS@ to the appropriate variables in his
# Makefile.am files

AC_DEFUN([AM_PATH_SNDFILE], [
	AC_ARG_WITH(sndfile-prefix,
		[  --with-sndfile-prefix=DIR   Prefix where Sndfile library was installed (optional)],
		[sndfile_prefix="$withval"], [sndfile_prefix=""])

	SNDFILE_CPPFLAGS=""
	SNDFILE_LIBS="-lsndfile"

	if test "x$sndfile_prefix" != "x" ; then
		SNDFILE_CPPFLAGS="-I$sndfile_prefix/include"
		SNDFILE_LIBS="-L$sndfile_prefix/lib $SNDFILE_LIBS"
	fi

	saved_CPPFLAGS="$CPPFLAGS"
	saved_LIBS="$LIBS"

	CPPFLAGS="$CPPFLAGS $SNDFILE_CPPFLAGS"
		LIBS="$LIBS $SNDFILE_LIBS"

	dnl make sure sndfile.h header file exists
	AC_CHECK_HEADER([sndfile.h], [

		dnl make sure libsndfile is linkable
		AC_LINK_IFELSE([
			AC_LANG_PROGRAM([[#include <sndfile.h>]],
				[[SF_INFO info; sf_format_check(&info);]])], [

			dnl libsndfile found
			AC_SUBST(SNDFILE_CPPFLAGS)
			AC_SUBST(SNDFILE_LIBS)
		], [
			AC_MSG_FAILURE([invalid Sndfile installation])])
	], [
		AC_MSG_ERROR([it seems you have no Sndfile library installed])])

	CPPFLAGS="$saved_CPPFLAGS"
	LIBS="$saved_LIBS"
])
