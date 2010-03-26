## Process this file with automake to produce Makefile.in

bin_PROGRAMS = multispeech

AM_CPPFLAGS = -DSYSCONF_DIR=\"$(sysconfdir)\" -DDATA_DIR=\"$(datadir)\" \
	-pthread -I$(top_srcdir)/src/mscore @BOOST_CPPFLAGS@ @BOBCAT_CPPFLAGS@ \
	@SNDFILE_CPPFLAGS@ @PORTAUDIOCPP_CPPFLAGS@ @SOUNDTOUCH_CXXFLAGS@
AM_CXXFLAGS = -O2 -Wall -Wno-sign-compare
AM_LDFLAGS = -pthread
multispeech_LDADD = $(top_srcdir)/src/mscore/libmultispeech.la @BOOST_LIBS@ @BOBCAT_LIBS@ -lm

multispeech_SOURCES = multispeech.cpp frontend.cpp frontend.hpp \
	inline_parser.cpp inline_parser.hpp \
	multispeech_voices.cpp multispeech_voices.hpp \
	dtk_voices.cpp dtk_voices.hpp
