## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libmultispeech.la

AM_CPPFLAGS = -DSYSCONF_DIR=\"$(sysconfdir)\" -DDATA_DIR=\"$(datadir)\" \
	-pthread @BOOST_CPPFLAGS@ @BOBCAT_CPPFLAGS@ @SNDFILE_CPPFLAGS@ \
	@PORTAUDIOCPP_CPPFLAGS@ @SOUNDTOUCH_CXXFLAGS@
AM_CXXFLAGS = -O2 -Wall -Wno-sign-compare -std=c++0x
AM_LDFLAGS = -pthread
libmultispeech_la_LIBADD = @BOOST_LIBS@ @BOBCAT_LIBS@ @SNDFILE_LIBS@ \
	@PORTAUDIOCPP_LIBS@ @SOUNDTOUCH_LIBS@ -lm

libmultispeech_la_SOURCES = exec_queue.hpp config.cpp config.hpp \
	iconv_codecvt.cpp iconv_codecvt.hpp strcvt.cpp strcvt.hpp \
	text_filter.cpp text_filter.hpp \
	language_description.cpp language_description.hpp \
	audioplayer.cpp audioplayer.hpp \
	soundfile.cpp soundfile.hpp singleton.hpp \
	sound_processor.cpp sound_processor.hpp \
	loudspeaker.cpp loudspeaker.hpp \
	file_player.cpp file_player.hpp \
	tone_generator.cpp tone_generator.hpp \
	sound_manager.cpp sound_manager.hpp \
	session.cpp session.hpp \
	pipeline.cpp pipeline.hpp server.cpp server.hpp \
	speech_engine.cpp speech_engine.hpp \
	job.cpp job.hpp polyglot.cpp polyglot.hpp \
	English.cpp English.hpp Russian.cpp Russian.hpp \
	mbrola.cpp mbrola.hpp espeak.cpp espeak.hpp \
	freephone.cpp freephone.hpp ru_tts.cpp ru_tts.hpp \
	user_tts.cpp user_tts.hpp startup.cpp
