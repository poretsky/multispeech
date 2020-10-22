## Process this file with automake to produce Makefile.in

bin_PROGRAMS = multispeech

AM_CPPFLAGS = -DSYSCONF_DIR=\"$(sysconfdir)\" -DDATA_DIR=\"$(datadir)\" \
	-I$(top_srcdir)/src/core @BOOST_CPPFLAGS@ @BOBCAT_CPPFLAGS@ @SNDFILE_CPPFLAGS@ \
	@PORTAUDIOCPP_CPPFLAGS@ @PULSEAUDIO_CPPFLAGS@ @SOUNDTOUCH_CXXFLAGS@
AM_CXXFLAGS = -Wall -Wno-sign-compare
AM_LDFLAGS = -pthread @BOOST_LDFLAGS@
multispeech_LDADD = $(top_srcdir)/src/core/libmultispeech.la @BOOST_FILESYSTEM_LIB@ @BOOST_IOSTREAMS_LIB@ \
	@BOOST_PROGRAM_OPTIONS_LIB@ @BOOST_REGEX_LIB@ \
	@BOOST_SYSTEM_LIB@ @BOOST_THREAD_LIB@ \
	@BOBCAT_LIBS@ @SNDFILE_LIBS@ @PORTAUDIOCPP_LIBS@ \
	@PULSEAUDIO_LIBS@ @SOUNDTOUCH_LIBS@ -lrt -lm

multispeech_SOURCES = multispeech.cpp frontend.cpp frontend.hpp \
	inline_parser.cpp inline_parser.hpp \
	multispeech_voices.cpp multispeech_voices.hpp \
	dtk_voices.cpp dtk_voices.hpp

MAINTAINERCLEANFILES = Makefile.in
