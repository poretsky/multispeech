## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libmultispeech.la

AM_CPPFLAGS = -DSYSCONF_DIR=\"$(sysconfdir)\" -DDATA_DIR=\"$(datadir)\" \
	@BOOST_CPPFLAGS@ @BOBCAT_CPPFLAGS@ @SNDFILE_CPPFLAGS@ \
	@PORTAUDIOCPP_CPPFLAGS@ @PULSEAUDIO_CPPFLAGS@ @SOUNDTOUCH_CXXFLAGS@
AM_CXXFLAGS = -Wall -Wno-sign-compare
AM_LDFLAGS = -pthread @BOOST_LDFLAGS@
libmultispeech_la_LIBADD = @BOOST_FILESYSTEM_LIB@ @BOOST_IOSTREAMS_LIB@ \
	@BOOST_LOCALE_LIB@ @BOOST_PROGRAM_OPTIONS_LIB@ @BOOST_REGEX_LIB@ \
	@BOOST_SYSTEM_LIB@ @BOOST_THREAD_LIB@ \
	@BOBCAT_LIBS@ @SNDFILE_LIBS@ @PORTAUDIOCPP_LIBS@ \
	@PULSEAUDIO_LIBS@ @SOUNDTOUCH_LIBS@ -lrt -lm
libmultispeech_la_LDFLAGS = -version-info 4:0:1

if HAVE_VSCRIPT
libmultispeech_la_LDFLAGS += $(VSCRIPT_LDFLAGS),multispeech.vscript
endif

libmultispeech_la_SOURCES = exec_queue.hpp config.cpp config.hpp \
	voice_params.cpp voice_params.hpp \
	iconv_codecvt.cpp iconv_codecvt.hpp strcvt.cpp strcvt.hpp \
	text_filter.cpp text_filter.hpp \
	language_description.cpp language_description.hpp \
	audioplayer.cpp audioplayer.hpp \
	soundfile.cpp soundfile.hpp \
	sound_processor.cpp sound_processor.hpp \
	loudspeaker.cpp loudspeaker.hpp \
	file_player.cpp file_player.hpp \
	tone_generator.cpp tone_generator.hpp \
	sound_manager.cpp sound_manager.hpp \
	pipeline.cpp pipeline.hpp \
	speech_server.cpp speech_server.hpp \
	speech_engine.cpp speech_engine.hpp \
	polyglot.cpp polyglot.hpp \
	English.cpp English.hpp Russian.cpp Russian.hpp \
	German.cpp German.hpp French.cpp French.hpp \
	Spanish.cpp Spanish.hpp Italian.cpp Italian.hpp \
	Portuguese.cpp Portuguese.hpp \
	mbrola.cpp mbrola.hpp espeak.cpp espeak.hpp \
	freephone.cpp freephone.hpp ru_tts.cpp ru_tts.hpp \
	user_tts.cpp user_tts.hpp \
	multispeech.cpp multispeech.hpp \
	sysconfig.hpp

EXTRA_DIST = multispeech.vscript
MAINTAINERCLEANFILES = Makefile.in
