# Check and configure paths for libportaudiocpp
#
# Designed especially for the Multispeech project
# by Igor B. Poretsky <poretsky@mlbox.ru>
#
#	Adds the following arguments to configure:
# --with-portaudio-prefix=DIR
#
# A user of libportaudiocpp should add @PORTAUDIOCPP_LIBS@ and
# @PORTAUDIOCPP_CPPFLAGS@ to the appropriate variables in his
# Makefile.am files

AC_DEFUN([AM_PATH_PORTAUDIOCPP], [
	AC_ARG_WITH(portaudio-prefix,
		[  --with-portaudio-prefix=DIR   Prefix where PortAudio library was installed (optional)],
		[portaudio_prefix="$withval"], [portaudio_prefix=""])

	saved_CPPFLAGS="$CPPFLAGS"
	saved_LIBS="$LIBS"

	CPPFLAGS="$CPPFLAGS -I$portaudio_prefix/include"
		LIBS="$LIBS -L$portaudio_prefix/lib -lportaudiocpp"

	dnl make sure PortaudioCpp.hxx header file exists
	AC_CHECK_HEADER([portaudiocpp/PortAudioCpp.hxx], [

		dnl make sure libportaudiocpp is linkable
		AC_LINK_IFELSE([
			AC_LANG_PROGRAM([[#include <portaudiocpp/PortAudioCpp.hxx>]],
				[[portaudio::AutoSystem audio;]])], [

			dnl libportaudiocpp found
			AC_SUBST(PORTAUDIOCPP_CPPFLAGS, [-I$portaudio_prefix/include])
			AC_SUBST(PORTAUDIOCPP_LIBS, ["-L$portaudio_prefix/lib -lportaudiocpp"])
		], [
			AC_MSG_FAILURE([incomplete or broken PortAudio installation])])
	], [
		AC_MSG_ERROR([at least PortAudio 19 is required])])

	CPPFLAGS="$saved_CPPFLAGS"
	LIBS="$saved_LIBS"
])
