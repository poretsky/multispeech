## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc
doc_DATA = AUTHORS README
sysconf_DATA = multispeech.conf
EXTRA_DIST = multispeech.conf.in debian
CLEANFILES = multispeech.conf
MAINTAINERCLEANFILES = aclocal.m4 compile config.guess config.sub configure \
	COPYING depcomp INSTALL install-sh Makefile.in missing sysconfig.h.in

%.conf: %.conf.in
	sed -e "s|<sysconfdir>|$(sysconfdir)|g" -e "s|<datadir>|$(datadir)|g" $< > $@
