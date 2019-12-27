# Multilingual speech server

Multispeech was primarily designed as a multilingual speech server for
[Emacspeak](https://github.com/tvraman/emacspeak),
but it can be useful in some other circumstances as well,
when multilingual speech feedback is needed.
For instance, it can work in conjunction with
[Speech Dispatcher](https://github.com/brailcom/speechd)
as its backend module.

Multispeech utilizes third party speech synthesis software to perform
actual TTS transformation. Being capable to detect language by text
nature it can automatically choose an appropriate TTS for each one.
For the moment English, German, French, Italian, Spanish, Portuguese
and Russian languages are supported.

To bootstrap the project do the following:

```bash
$ git clone https://github.com/poretsky/multispeech.git
$ cd multispeech
$ autoreconf -if
```

GNU [Automake](https://www.gnu.org/software/automake/)
and [Autoconf](https://www.gnu.org/software/autoconf/)
tools are necessary for this step along with additional macros from
[Autoconf Archive](https://www.gnu.org/software/autoconf-archive/).
 Afterwards README and INSTALL files may be consulted for further
instructions and full list of build dependencies.

Or alternatively you can download ready to use bootstrapped
source distribution from
[releases](https://github.com/poretsky/multispeech/releases)
page.
