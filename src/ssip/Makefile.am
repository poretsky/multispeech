## Process this file with automake to produce Makefile.in

bin_PROGRAMS = sd_multispeech

AM_CPPFLAGS = -DSYSCONF_DIR=\"$(sysconfdir)\" -DDATA_DIR=\"$(datadir)\" \
	-I$(top_srcdir)/src/core @BOOST_CPPFLAGS@ @BOBCAT_CPPFLAGS@ @PORTAUDIOCPP_CPPFLAGS@
AM_CXXFLAGS = -Wall -Wno-sign-compare
AM_LDFLAGS = -pthread @BOOST_LDFLAGS@
sd_multispeech_LDADD = $(top_srcdir)/src/core/libmultispeech.la \
	@BOOST_FILESYSTEM_LIB@ @BOOST_REGEX_LIB@ \
	@BOOST_SYSTEM_LIB@ @BOOST_THREAD_LIB@ \
	@BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOBCAT_LIBS@ @PORTAUDIOCPP_LIBS@ -lrt -lm

sd_multispeech_SOURCES = sd_multispeech.cpp \
	frontend.cpp frontend.hpp \
	spd_settings.cpp spd_settings.hpp

MAINTAINERCLEANFILES = Makefile.in
