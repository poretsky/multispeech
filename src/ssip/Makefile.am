## Process this file with automake to produce Makefile.in

bin_PROGRAMS = multispeechd

AM_CPPFLAGS = -DSYSCONF_DIR=\"$(sysconfdir)\" -DDATA_DIR=\"$(datadir)\" \
	-pthread -I$(top_srcdir)/src @BOOST_CPPFLAGS@ @BOBCAT_CPPFLAGS@ \
	@SNDFILE_CPPFLAGS@ @PORTAUDIOCPP_CPPFLAGS@ @SOUNDTOUCH_CXXFLAGS@
AM_CXXFLAGS = -O2 -Wall -Wno-sign-compare
AM_LDFLAGS = -pthread
multispeechd_LDADD = $(top_srcdir)/src/mscore/libmultispeech.la @BOOST_LIBS@ @BOBCAT_LIBS@ -lm

multispeechd_SOURCES = multispeechd.cpp server.cpp server.hpp \
	session.cpp session.hpp message.cpp message.hpp \
	connection.cpp connection.hpp proxy.hpp \
	parser.cpp parser.hpp
