## Process this file with automake to produce Makefile.in

man_MANS = multispeech.1 multispeech.conf.5
CLEANFILES = multispeech.1 multispeech.conf.5
MAINTAINERCLEANFILES = Makefile.in
dist_doc_DATA = interface.txt prehistory.ChangeLog
EXTRA_DIST = multispeech.man multispeech.conf.man

multispeech.1: multispeech.man
	sed -e "s|<sysconfdir>|$(sysconfdir)|g" $< > $@

multispeech.conf.5: multispeech.conf.man
	sed -e "s|<sysconfdir>|$(sysconfdir)|g" -e "s|<datadir>|$(datadir)|g" $< > $@
