## Process this file with automake to produce Makefile.in

man_MANS = multispeech.1 multispeech.conf.5
CLEANFILES = multispeech.1 multispeech.conf.5
dist_doc_DATA = historical.txt
EXTRA_DIST = multispeech.man multispeech.conf.man

%.1 %.5: %.man
	sed -e "s|<sysconfdir>|$(sysconfdir)|g" -e "s|<datadir>|$(datadir)|g" $< > $@
