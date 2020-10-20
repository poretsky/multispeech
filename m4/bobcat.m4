# Check and configure paths for libbobcat
#
# Designed especially for the Multispeech project
# by Igor B. Poretsky <poretsky@mlbox.ru>
#
#	Adds the following arguments to configure:
# --with-bobcat-prefix=DIR
#
# A user of libbobcat should add @BOBCAT_LIBS@ and
# @BOBCAT_CPPFLAGS@ to the appropriate variables in his
# Makefile.am files

AC_DEFUN([AM_PATH_BOBCAT], [
	AC_ARG_WITH(bobcat-prefix,
		[  --with-bobcat-prefix=DIR   Prefix where Bobcat library was installed (optional)],
		[bobcat_prefix="$withval"], [bobcat_prefix=""])

	BOBCAT_CPPFLAGS=""
	BOBCAT_LIBS="-lbobcat"

	if test "x$bobcat_prefix" != "x" ; then
		BOBCAT_CPPFLAGS="-I$bobcat_prefix/include"
		BOBCAT_LIBS="-L$bobcat_prefix/lib $BOBCAT_LIBS"
	fi

	saved_CPPFLAGS="$CPPFLAGS"
	saved_LIBS="$LIBS"

	CPPFLAGS="$CPPFLAGS $BOBCAT_CPPFLAGS"
	LIBS="$LIBS $BOBCAT_LIBS"

	dnl make sure all necessary header files exist
	AC_CHECK_HEADERS([bobcat/ofdstreambuf bobcat/ofdbuf], [BOBCAT_HEADERS_OK=yes])
	AC_CHECK_HEADERS([bobcat/string bobcat/redirector bobcat/fork bobcat/pipe bobcat/syslogstream bobcat/cmdfinder], [], [BOBCAT_HEADERS_OK=no])

	if test "x$BOBCAT_HEADERS_OK" = "xyes" ; then

		dnl make sure libbobcat is linkable
		AC_LINK_IFELSE([
			AC_LANG_PROGRAM([[#if HAVE_BOBCAT_OFDSTREAMBUF
					#include <bobcat/ofdstreambuf>
					#elif HAVE_BOBCAT_OFDBUF
					#include <bobcat/ofdbuf>
					#define OFdStreambuf OFdBuf
					#endif]],
				[[FBB::OFdStreambuf stream(1);]])], [

			dnl libbobcat found
			AC_SUBST(BOBCAT_CPPFLAGS)
			AC_SUBST(BOBCAT_LIBS)
		], [
			AC_MSG_FAILURE([invalid Bobcat installation])])

	else
		AC_MSG_ERROR([incomplete or broken Bobcat installation])
	fi

	CPPFLAGS="$saved_CPPFLAGS"
	LIBS="$saved_LIBS"
])
