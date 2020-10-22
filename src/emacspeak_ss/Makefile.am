## Process this file with automake to produce Makefile.in

bin_PROGRAMS = multispeech

AM_CPPFLAGS = -DSYSCONF_DIR=\"$(sysconfdir)\" -DDATA_DIR=\"$(datadir)\" \
	-I$(top_srcdir)/src/core @BOOST_CPPFLAGS@ @BOBCAT_CPPFLAGS@ @PORTAUDIOCPP_CPPFLAGS@
AM_CXXFLAGS = -Wall -Wno-sign-compare
AM_LDFLAGS = -pthread @BOOST_LDFLAGS@
multispeech_LDADD = $(top_srcdir)/src/core/libmultispeech.la \
	@BOOST_REGEX_LIB@ @BOOST_SYSTEM_LIB@ @BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOBCAT_LIBS@ @PORTAUDIOCPP_LIBS@ -lrt -lm

multispeech_SOURCES = multispeech.cpp frontend.cpp frontend.hpp \
	inline_parser.cpp inline_parser.hpp \
	multispeech_voices.cpp multispeech_voices.hpp \
	dtk_voices.cpp dtk_voices.hpp

MAINTAINERCLEANFILES = Makefile.in
