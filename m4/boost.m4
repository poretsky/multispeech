# Check and configure paths for libboost
#
# Designed especially for the Multispeech project
# by Igor B. Poretsky <poretsky@mlbox.ru>
#
#	Adds the following arguments to configure:
# --with-boost-prefix=DIR
#
# A user of libboost should add @BOOST_CPPFLAGS@ and @BOOST_LIBS@
# to the appropriate variables in his Makefile.am files

AC_DEFUN([AM_PATH_BOOST], [
	AC_ARG_WITH(boost-prefix,
		[  --with-boost-prefix=DIR   Prefix where Boost library was installed (optional)],
		[boost_prefix="$withval"], [boost_prefix=""])

	saved_CPPFLAGS="$CPPFLAGS"
	saved_CXXFLAGS="$CXXFLAGS"
	saved_LIBS="$LIBS"

	CPPFLAGS="$CPPFLAGS -I$boost_prefix/include"
	CXXFLAGS="$CXXFLAGS -pthread"
	LIBS="$LIBS -L$boost_prefix/lib"

	dnl make sure all necessary header files exist
	AC_CHECK_HEADERS([ \
		boost/algorithm/string.hpp \
		boost/algorithm/string/regex_find_format.hpp \
		boost/any.hpp \
		boost/assign.hpp \
		boost/foreach.hpp \
		boost/function.hpp \
		boost/lexical_cast.hpp \
		boost/numeric/conversion/cast.hpp \
		boost/range.hpp \
		boost/shared_ptr.hpp \
	], [

		BOOST_LIBS="-L$boost_prefix/lib"

		dnl check for thread support
		AC_CHECK_HEADERS([boost/thread/thread.hpp boost/thread/mutex.hpp boost/thread/condition.hpp], [
			LIBS="$saved_LIBS $BOOST_LIBS -lboost_thread"
			AC_LINK_IFELSE([
				AC_LANG_PROGRAM([[#include <boost/thread/thread.hpp>]],
				[[boost::thread::yield();]])], [], [

				dnl fail to link
				AC_MSG_FAILURE([broken Boost thread support library])])], [

			dnl not found
			AC_MSG_ERROR([no required thread support found in current Boost library installation])])

		dnl check for regex support
		AC_CHECK_HEADER([boost/regex.hpp], [
			LIBS="$saved_LIBS $BOOST_LIBS -lboost_regex-mt"
			AC_LINK_IFELSE([
				AC_LANG_PROGRAM([[#include <boost/regex.hpp>]],
				[[boost::wregex re(L"pattern", boost::regex::normal);]])], [], [

				dnl fail to link
				AC_MSG_FAILURE([broken Boost regex support library])])], [

			dnl not found
			AC_MSG_ERROR([no regex support found in current Boost library installation])])

		dnl check for file system support
		AC_CHECK_HEADERS([boost/filesystem.hpp boost/filesystem/fstream.hpp], [
			LIBS="$saved_LIBS $BOOST_LIBS -lboost_filesystem-mt"
			AC_LINK_IFELSE([
				AC_LANG_PROGRAM([[#include <boost/filesystem.hpp>]],
				[[boost::filesystem::path file("/dev/null");]])], [], [

				dnl fail to link
				AC_MSG_FAILURE([broken Boost file system support library])])], [

			dnl not found
			AC_MSG_ERROR([no required file system support found in current Boost library installation])])

		dnl check for iostreams support
		AC_CHECK_HEADERS([boost/iostreams/code_converter.hpp boost/iostreams/filtering_stream.hpp boost/iostreams/filter/line.hpp boost/iostreams/device/back_inserter.hpp boost/iostreams/device/array.hpp], [
			LIBS="$saved_LIBS $BOOST_LIBS -lboost_iostreams-mt"
			AC_LINK_IFELSE([
				AC_LANG_PROGRAM([[#include <boost/iostreams/filtering_stream.hpp>]],
				[[boost::iostreams::filtering_wostream filter;]])], [], [

				dnl fail to link
				AC_MSG_FAILURE([broken Boost iostreams support library])])], [

			dnl not found
			AC_MSG_ERROR([no required iostreams support found in current Boost library installation])])

		dnl check for program options support
		AC_CHECK_HEADER([boost/program_options.hpp], [
			LIBS="$saved_LIBS $BOOST_LIBS -lboost_program_options-mt"
			AC_LINK_IFELSE([
				AC_LANG_PROGRAM([[#include <boost/program_options.hpp>]],
				[[boost::program_options::variables_map options;]])], [], [

				dnl fail to link
				AC_MSG_FAILURE([broken Boost program options support library])])], [

			dnl not found
			AC_MSG_ERROR([no program options support found in current Boost library installation])])

		dnl libboost found
		AC_SUBST(BOOST_CPPFLAGS, [-I$boost_prefix/include])
		AC_SUBST(BOOST_LIBS, ["-L$boost_prefix/lib -lboost_thread -lboost_iostreams-mt -lboost_regex-mt -lboost_program_options-mt -lboost_filesystem-mt"])
	], [
		AC_MSG_ERROR([incomplete or broken Boost library installation])])

	CPPFLAGS="$saved_CPPFLAGS"
	CXXFLAGS="$saved_CXXFLAGS"
	LIBS="$saved_LIBS"
])