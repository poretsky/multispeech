# Check and configure paths for libpulseaudio
#
# Designed especially for the Multispeech project
# by Igor B. Poretsky <poretsky@mlbox.ru>
#
#	Adds the following arguments to configure:
# --with-pulseaudio-prefix=DIR
#
# A user of libpulseaudio should add @PULSEAUDIO_LIBS@ and
# @PULSEAUDIO_CPPFLAGS@ to the appropriate variables in his
# Makefile.am files

AC_DEFUN([AM_PATH_PULSEAUDIO], [
	AC_ARG_WITH(pulseaudio-prefix,
		[  --with-pulseaudio-prefix=DIR   Prefix where PulseAudio library was installed (optional)],
		[pulseaudio_prefix="$withval"], [pulseaudio_prefix=""])

	PULSEAUDIO_CPPFLAGS=""
	PULSEAUDIO_LIBS="-lpulse-simple -lpulse"

	if test "x$pulseaudio_prefix" != "x" ; then
		PULSEAUDIO_CPPFLAGS="-I$pulseaudio_prefix/include"
		PULSEAUDIO_LIBS="-L$pulseaudio_prefix/lib $PULSEAUDIO_LIBS"
	fi

	saved_CPPFLAGS="$CPPFLAGS"
	saved_LIBS="$LIBS"

	CPPFLAGS="$CPPFLAGS $PULSEAUDIO_CPPFLAGS"
		LIBS="$LIBS $PULSEAUDIO_LIBS"

	dnl make sure pulse/simple.h header file exists
	AC_CHECK_HEADER([pulse/simple.h], [

		dnl make sure libpulseaudio is linkable
		AC_LINK_IFELSE([
			AC_LANG_PROGRAM([[#include <pulse/simple.h>]],
				[[pa_sample_spec spec; pa_sample_spec_valid(&spec);]])], [

			dnl libpulseaudio found
			AC_SUBST(PULSEAUDIO_CPPFLAGS)
			AC_SUBST(PULSEAUDIO_LIBS)
		], [
			AC_MSG_FAILURE([invalid PulseAudio library installation])])
	], [
		AC_MSG_ERROR([it seems you have no PulseAudio library installed])])

	CPPFLAGS="$saved_CPPFLAGS"
	LIBS="$saved_LIBS"
])
