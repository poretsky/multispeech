; This file loads Mbrola specific stuff
; and in additional it contains some my preferences
; particularly concerning Emacspeak
; Igor B. Poretsky <root@goga.energo.ru>

(setq emacspeak-speak-default-os-coding-system 'cyrillic-koi8)
(setq default-enable-multibyte-characters t)
(set-process-coding-system dtk-speaker-process 'cyrillic-koi8 'cyrillic-koi8)

(load-library "mbrola-css-speech")
(load-library "mbrola-voices")

(defsubst dtk-interp-sound (sound)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process
                       (format "p {%s}\n" sound )))

(defsubst dtk-sound (sound)
  "Speak a letter"
  (declare (special dtk-speaker-process
                    dtk-speak-server-initialized
                    dtk-quiet ))
  (unless dtk-quiet
    (when dtk-speak-server-initialized
      (dtk-interp-sound  sound ))))

(defsubst emacspeak-play-auditory-icon-via-speech-server (sound-name)
  "Produce auditory icon sound-name via speech-server.
Sound is produced only if emacspeak-use-auditory-icons is true.
See command emacspeak-toggle-auditory-icons bound to \\[emacspeak-toggle-auditory-icons ]."
  (declare (special  emacspeak-use-auditory-icons))
  (and emacspeak-use-auditory-icons
       (dtk-sound (emacspeak-get-sound-filename sound-name ))))

;;}}}
;;{{{  setup play function

(setq emacspeak-auditory-icon-function 'emacspeak-play-auditory-icon-via-speech-server)
(setq-default emacspeak-use-auditory-icons t)
(setq emacspeak-speak-messages nil)

;;;  Russian stuf loading
(setq emacspeak-unspeakable-rule "^[^0-9a-zA-Z\243\263\300-\377\xe30-\xe6f\xe21\xe71]+$")
(setq-default dtk-speak-nonprinting-chars nil)
(let ((coding-system-for-read 'raw-text))
  (load-library "Russian-spelling"))
(let ((coding-system-for-read 'cyrillic-koi8))
  (load-library "Russian-spelling"))
