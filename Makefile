# This is a Makefile for the all this stuf
# You should glance at the scripts subdirectory
# and make sure there is all right. :-)
#
# binaries.
BINS = src/freephone/freephone src/multilingual/speech_server src/tones/tones
#
# documentation files.
DOCS = src/tones/tones.1

all: speech_server freephone tones letters
	mkdir -p binaries
	cp -f $(BINS) binaries
	mkdir -p docs
	cp -f $(DOCS) docs

# Build speech_server
speech_server:
	cd src/multilingual; $(MAKE) all

# build our modified version of freephone
freephone:
	cd src/freephone; $(MAKE) all

# build the tones prg
tones:
	cd src/tones; $(MAKE) all

# generates the pre-synthesized letters.
# dot.raw is the last "letter", so let's suppose if that one exists, all
# others have been generated as well
letters: letters_br letters_de letters_en letters_es letters_fr letters_ru

letters_br: letters/br/dot.raw
letters_de: letters/de/dot.raw
letters_en: letters/en/dot.raw
letters_es: letters/es/dot.raw
letters_fr: letters/fr/dot.raw
letters_ru: letters/ru/dot.raw

# Brazilian letters
letters/br/dot.raw:
	cd letters/br; cat letters.lst | awk -f ../../scripts/generate_letters tts=../../scripts/tts/br rate=0.41 default_pitch=1.0 high_pitch=1.25 volume=0.8 frequency=16000

# German letters
letters/de/dot.raw:
	cd letters/de; cat letters.lst | awk -f ../../scripts/generate_letters tts=../../scripts/tts/de rate=0.52 default_pitch=1.0 high_pitch=1.25 volume=2.2 frequency=16000

# English letters
letters/en/dot.raw:
	cd letters/en; cat letters.lst | awk -f ../../scripts/generate_letters tts=../../scripts/tts/en rate=0.5 default_pitch=1.0 high_pitch=1.25 volume=2.0 frequency=16000

# Spanish letters
letters/es/dot.raw:
	cd letters/es; cat letters.lst | awk -f ../../scripts/generate_letters tts=../../scripts/tts/es rate=0.5 default_pitch=1.0 high_pitch=1.25 volume=2.0 frequency=16000

# French letters
letters/fr/dot.raw:
	cd letters/fr; cat letters.lst | awk -f ../../scripts/generate_letters tts=../../scripts/tts/fr rate=0.58 default_pitch=1.0 high_pitch=1.25 volume=0.7 frequency=16000

# Russian letters
letters/ru/dot.raw:
	cd letters/ru; cat letters.lst | awk -f ../../scripts/generate_letters tts=../../scripts/tts/ru rate=0.0 default_pitch=0.2 high_pitch=0.4 volume=0.8 frequency=10000

clean:
	cd src/freephone; $(MAKE) clean
	cd src/tones; $(MAKE) clean
	cd src/multilingual; $(MAKE) clean
