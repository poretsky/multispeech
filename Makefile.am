## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc
doc_DATA = AUTHORS README TODO
sysconf_DATA = multispeech.conf
EXTRA_DIST = bootstrap multispeech.conf.in debian
CLEANFILES = multispeech.conf

%.conf: %.conf.in
	sed -e "s|<sysconfdir>|$(sysconfdir)|g" -e "s|<datadir>|$(datadir)|g" $< > $@
