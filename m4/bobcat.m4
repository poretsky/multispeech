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

	saved_CPPFLAGS="$CPPFLAGS"
	saved_LIBS="$LIBS"

	CPPFLAGS="$CPPFLAGS -I$bobcat_prefix/include"
		LIBS="$LIBS -L$bobcat_prefix/lib -lbobcat"

	dnl make sure all necessary header files exist
	AC_CHECK_HEADERS([bobcat/string bobcat/redirector bobcat/fork bobcat/pipe bobcat/ofdstreambuf bobcat/syslogstream bobcat/cmdfinder bobcat/ifdstream bobcat/ofdstream bobcat/serversocket], [

		dnl make sure libbobcat is linkable
		AC_LINK_IFELSE([
			AC_LANG_PROGRAM([[#include <bobcat/ofdstreambuf>]],
				[[FBB::OFdStreambuf stream(1);]])], [

			dnl libbobcat found
			AC_SUBST(BOBCAT_CPPFLAGS, [-I$bobcat_prefix/include])
			AC_SUBST(BOBCAT_LIBS, ["-L$bobcat_prefix/lib -lbobcat"])
		], [
			AC_MSG_FAILURE([invalid Bobcat installation])])
	], [
		AC_MSG_ERROR([incomplete or broken Bobcat installation])])

	CPPFLAGS="$saved_CPPFLAGS"
	LIBS="$saved_LIBS"
])
