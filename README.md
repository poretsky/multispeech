# Multilingual speech server

Multispeech was primarily designed as a multilingual speech server
for Emacspeak, but it can be useful in some other circumstances
as well, when multilingual speech feedback is needed. It utilizes
third party speech synthesis software to perform actual TTS
transformation. Being capable to detect language by text nature it
can automatically choose an appropriate TTS for each one. For the
moment English, German, French, Italian, Spanish, Brazilian
Portuguese and Russian languages are supported.

To bootstrap the project do the following:

```bash
$ git clone https://github.com/poretsky/multispeech.git
$ cd multispeech
$ ./bootstrap
```

GNU [Automake](https://www.gnu.org/software/automake/)
and [Autoconf](https://www.gnu.org/software/autoconf/)
tools are necessary for this step along with additional macros from
[Autoconf Archive](https://www.gnu.org/software/autoconf-archive/).
 Afterwards README and INSTALL files may be consulted for further
instructions and full list of build dependencies.
