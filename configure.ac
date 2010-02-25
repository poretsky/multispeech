# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT([Multispeech], [2.3.2])
AC_CONFIG_SRCDIR([src/multispeech.cpp])
AC_CONFIG_HEADERS([sysconfig.h])
                            AM_INIT_AUTOMAKE([foreign])

AC_SUBST(VERSION)

AC_REQUIRE_AUX_FILE([INSTALL])
AC_REQUIRE_AUX_FILE([COPYING])

# Guess environment.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

# Language choice.
AC_LANG([C++])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for libraries.
AM_PATH_SOUNDTOUCH([], [], [AC_MSG_ERROR([the SoundTouch library is mandatory])])
AM_PATH_PORTAUDIOCPP
AM_PATH_SNDFILE
AM_PATH_BOBCAT
AM_PATH_BOOST

# Output results.
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])
AC_OUTPUT
