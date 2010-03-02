# -*- coding: utf-8 -*-
# Place this file into ~/.orca and edit according your preferences.

# At first allow Multispeech as one of speech backends.
import orca.espeechfactory
orca.espeechfactory._codeTable['multispeech'] = 'dectalk'

# Adjust some speech parameters.
orca.espeechfactory.SpeechServer.config["rate"] = 20
orca.espeechfactory.SpeechServer.config["punctuations"] = 'some'

# Uncomment following two lines if you want Multispeech
# to be the default speech backend for Orca.
# These settings override GUI choice.
#orca.settings.speechServerFactory = 'orca.espeechfactory'
#orca.settings.speechServerInfo = ['multispeech', 'multispeech']
