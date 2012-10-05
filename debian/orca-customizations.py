# -*- coding: utf-8 -*-
# Place this file into ~/.orca and edit it according to your preferences.

# At first import Emacspeak speech services bridge.
import orca.espeechfactory

# Set default speech rate. It is actual while voice is not tuned.
orca.espeechfactory.SpeechServer.config["rate"] = 20

# Adjust punctuations mode. Since Emacspeak speech server
# doesn't support punctuations mode "most" we treat it here as "some".
if orca.settings.verbalizePunctuationStyle == orca.settings.PUNCTUATION_STYLE_ALL:
    orca.espeechfactory.SpeechServer.config["punctuations"] = 'all'
elif orca.settings.verbalizePunctuationStyle == orca.settings.PUNCTUATION_STYLE_MOST:
    orca.espeechfactory.SpeechServer.config["punctuations"] = 'some'
elif orca.settings.verbalizePunctuationStyle == orca.settings.PUNCTUATION_STYLE_SOME:
    orca.espeechfactory.SpeechServer.config["punctuations"] = 'some'
elif orca.settings.verbalizePunctuationStyle == orca.settings.PUNCTUATION_STYLE_NONE:
    orca.espeechfactory.SpeechServer.config["punctuations"] = 'none'

# Uncomment following two lines if you want Multispeech
# to be the default speech backend for Orca.
# These settings override GUI choice.
#orca.settings.speechServerFactory = 'orca.espeechfactory'
#orca.settings.speechServerInfo = ['multispeech', 'multispeech']

# Now inform Orca about the Multispeech speech server.
#
# Here wi have two choices: we can add Multispeech to the list
# of known speech servers or make it the only Emacspeak speech server
# to use. Uncomment one of the alternatives below.
#
# Add Multispeech to the list of known speech servers.
#orca.espeechfactory._codeTable['multispeech'] = 'dectalk'
#
# Make Multispeech the only available Emacspeak speech server.
orca.espeechfactory._codeTable = {'multispeech' : 'dectalk'}

# Create list of active Emacspeak speech servers for reference.
# Note that all available speech servers will be launched, but we
# should do it here because otherwise the stuff below will not work.
# So check carefully the previous stanza and make the right choice.
# Another alternative is to comment out unneeded speech servers
# in /usr/share/emacs/site-lisp/emacspeak/servers/.servers.
if orca.settings.speechServerFactory == 'orca.espeechfactory':
    orca.espeechfactory.SpeechServer.getSpeechServers()


# Setup custom key bindings.
#
# Here we will assign hot keys for switching punctuations mode.
# Originally Orca lacks this functionality,
# but I feel it to be pretty useful.
# Now we'll make following assignments:
# ORCA+HOME -- Punctuations all
# ORCA+PGUP -- Punctuations most
# ORCA+PGDN -- Punctuations some
# ORCA+END  -- Punctuations none
#
# This stuff can also be treated as an example of Orca scripting technique.

# At first, import all necessary modules.
import orca.input_event
import orca.keybindings
import orca.speech
import orca.orca_state

# Define functions for punctuations verbosity switching.
def setPunctAll(script, inputEvent=None):
    orca.settings.verbalizePunctuationStyle = orca.settings.PUNCTUATION_STYLE_ALL
    if orca.settings.speechServerFactory == 'orca.espeechfactory':
        s = orca.espeechfactory.SpeechServer.getSpeechServer(orca.settings.speechServerInfo)
        s.punctuations('all')
    orca.speech.speak("Punctuations all")
    return True
def setPunctSome(script, inputEvent=None):
    orca.settings.verbalizePunctuationStyle = orca.settings.PUNCTUATION_STYLE_SOME
    if orca.settings.speechServerFactory == 'orca.espeechfactory':
        s = orca.espeechfactory.SpeechServer.getSpeechServer(orca.settings.speechServerInfo)
        s.punctuations('some')
    orca.speech.speak("Punctuations some")
    return True
def setPunctMost(script, inputEvent=None):
    orca.settings.verbalizePunctuationStyle = orca.settings.PUNCTUATION_STYLE_MOST
    if orca.settings.speechServerFactory == 'orca.espeechfactory':
        s = orca.espeechfactory.SpeechServer.getSpeechServer(orca.settings.speechServerInfo)
        s.punctuations('some')
    orca.speech.speak("Punctuations most")
    return True
def setPunctNone(script, inputEvent=None):
    orca.settings.verbalizePunctuationStyle = orca.settings.PUNCTUATION_STYLE_NONE
    if orca.settings.speechServerFactory == 'orca.espeechfactory':
        s = orca.espeechfactory.SpeechServer.getSpeechServer(orca.settings.speechServerInfo)
        s.punctuations('none')
    orca.speech.speak("Punctuations none")
    return True

# Create key handlers.
setPunctAllHandler = orca.input_event.InputEventHandler(setPunctAll, "Punctuations all")
setPunctMostHandler = orca.input_event.InputEventHandler(setPunctMost, "Punctuations most")
setPunctSomeHandler = orca.input_event.InputEventHandler(setPunctSome, "Punctuations some")
setPunctNoneHandler = orca.input_event.InputEventHandler(setPunctNone, "Punctuations none")

# Create custom keymap and bind keys to our handlers.
myKeyBindings = orca.keybindings.KeyBindings()
myKeyBindings.add(orca.keybindings.KeyBinding(
    "Home",
    1 << orca.settings.MODIFIER_ORCA,
    1 << orca.settings.MODIFIER_ORCA,
    setPunctAllHandler))
myKeyBindings.add(orca.keybindings.KeyBinding(
    "Page_Up",
    1 << orca.settings.MODIFIER_ORCA,
    1 << orca.settings.MODIFIER_ORCA,
    setPunctMostHandler))
myKeyBindings.add(orca.keybindings.KeyBinding(
    "Page_Down",
    1 << orca.settings.MODIFIER_ORCA,
    1 << orca.settings.MODIFIER_ORCA,
    setPunctSomeHandler))
myKeyBindings.add(orca.keybindings.KeyBinding(
    "End",
    1 << orca.settings.MODIFIER_ORCA,
    1 << orca.settings.MODIFIER_ORCA,
    setPunctNoneHandler))

# Add our new keybindings to the Orca default map.
orca.settings.keyBindingsMap["default"] = myKeyBindings
