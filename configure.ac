# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT([Multispeech], [3.1.0trusty1])
AC_CONFIG_SRCDIR([src/core/server.cpp])
AC_CONFIG_HEADERS([sysconfig.h])
AM_INIT_AUTOMAKE([foreign])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_SUBST(VERSION)

AC_REQUIRE_AUX_FILE([INSTALL])
AC_REQUIRE_AUX_FILE([COPYING])

# Guess environment.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL

if test "$ac_cv_prog_cc_g" = "no" -o "$ac_cv_prog_cxx_g" = "no"
then AC_MSG_ERROR([No C/C++ compiler present])
fi

# Language choice.
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for required headers and libraries.
AC_CHECK_HEADERS([iconv.h unistd.h fcntl.h signal.h sys/types.h sys/wait.h sys/stat.h], [], AC_MSG_ERROR([Some system headers are not found]))
AC_CHECK_HEADERS([cmath cstdlib ctime cerrno cstring], [], AC_MSG_ERROR([Some StdC++ headers are not found]))
AC_CHECK_HEADERS([exception stdexcept string sstream iostream ostream memory queue vector map stack list typeinfo algorithm locale utility], [], AC_MSG_ERROR([Some STL headers are not found]))

AM_PATH_PORTAUDIOCPP
AM_PATH_PULSEAUDIO
AM_PATH_SNDFILE
AM_PATH_BOBCAT

AM_PATH_SOUNDTOUCH([], [], AC_MSG_ERROR([the SoundTouch library is mandatory]))
AX_BOOST_BASE([1.45], [], AC_MSG_ERROR([the Boost library is mandatory]))

AX_BOOST_FILESYSTEM
if test "$ax_cv_boost_filesystem" != "yes"
then AC_MSG_ERROR([Boost file system support is mandatory])
fi

AX_BOOST_IOSTREAMS
if test "$ax_cv_boost_iostreams" != "yes"
then AC_MSG_ERROR([Boost iostreams support is mandatory])
fi

AX_BOOST_PROGRAM_OPTIONS
if test "$ax_cv_boost_program_options" != "yes"
then AC_MSG_ERROR([Boost program options support is mandatory])
fi

AX_BOOST_REGEX
if test "$ax_cv_boost_regex" != "yes"
then AC_MSG_ERROR([Boost regex support is mandatory])
fi

AX_BOOST_SYSTEM
if test "$ax_cv_boost_system" != "yes"
then AC_MSG_ERROR([Boost system support is mandatory])
fi

AX_BOOST_THREAD
if test "$ax_cv_boost_thread" != "yes"
then AC_MSG_ERROR([Boost thread support is mandatory])
fi

# Output results.
AC_CONFIG_FILES([Makefile src/Makefile src/core/Makefile src/emacspeak_ss/Makefile src/ssip/Makefile doc/Makefile])
AC_OUTPUT
