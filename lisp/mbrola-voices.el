;;; $Id: mbrola-voices.el,v 1.1 2001/11/03 08:15:54 master Exp $
;;; $Author: master $ 
;;; Description:  Module to set up dtk voices and personalities
;;; Keywords: Voice, Personality, Dectalk
;;{{{  LCD Archive entry: 

;;; LCD Archive Entry:
;;; emacspeak| T. V. Raman |raman@adobe.com 
;;; A speech interface to Emacs |
;;; $Date: 2001/11/03 08:15:54 $ |
;;;  $Revision: 1.1 $ | 
;;; Location undetermined
;;;

;;}}}
;;{{{  Copyright:
;;;Copyright (C) 1995, 1996, 1997  T. V. Raman  Adobe Systems Incorporated
;;; Copyright (c) 1994, 1995 by Digital Equipment Corporation.
;;; All Rights Reserved. 
;;;
;;; This file is not part of GNU Emacs, but the same permissions apply.
;;;
;;; GNU Emacs is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2, or (at your option)
;;; any later version.
;;;
;;; GNU Emacs is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with GNU Emacs; see the file COPYING.  If not, write to
;;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

;;}}}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(require 'cl)
(declaim  (optimize  (safety 0) (speed 3)))
;;{{{  Introduction:

;;; This module defines the various voices used in voice-lock mode.
;;; This module is mbrola specific. 

;;}}}
;;; Code:
(require 'cl)
(declaim  (optimize  (safety 0) (speed 3)))
(eval-when-compile (load-library "cl-extra"))
;;{{{  voice table

(defvar dtk-default-voice 'paul
  "Default voice")

(defvar dtk-default-voice-string "[_: pi:1 fr:16000 ]"
  "Default dtk string for the default voice.")


(defvar dtk-voice-table (make-hash-table)
  "Association between symbols and strings to set dtk voices.
The string can set any dtk parameter. ")

(defsubst dtk-define-voice (name command-string)
  "Define a dtk voice named name, which is set by sending the string
command-string to the Dectalk. "
  (declare (special dtk-voice-table ))
   (setf (gethash name dtk-voice-table )
              command-string))

(defsubst dtk-get-voice-command (name)
  "Retrieve command string for this voice"
  (declare (special dtk-voice-table ))
  (or  (cl-gethash name dtk-voice-table)
        dtk-default-voice-string))

(defsubst dtk-voice-defined-p (name)
  (declare (special dtk-voice-table ))
  (cl-gethash name dtk-voice-table ))

(defsubst dtk-define-voice-alias (alias voice )
  (dtk-define-voice alias (dtk-get-voice-command voice )))

;;}}}
;;{{{ voice definitions

;;; the nine predefined voices: 
(dtk-define-voice 'paul "[_: pi:1 fr:16000 ]")
(dtk-define-voice 'harry "[_: pi:0.5 fr:16000 ]")
(dtk-define-voice 'dennis "[_: pi:0.7 fr:14000 ]")
(dtk-define-voice 'frank "[_: pi:0.7 fr:12000 ]")
(dtk-define-voice 'betty "[_: pi:1.4 fr:17000 ]")
(dtk-define-voice 'ursula "[_: pi:1.3 fr:16000 ]")
(dtk-define-voice 'rita "[_: pi:1.4 fr:18000 ]")
(dtk-define-voice 'wendy "[_: pi:1.5 fr:17000 ]")
(dtk-define-voice 'kit "[_: pi:2 fr:20000 ]")

;;; Modified voices:
;;; Modifications for paul:
(dtk-define-voice 'paul-bold "[_: ]")
(dtk-define-voice 'paul-italic
                  "[_: pi:1 vo:3.2 fr:17000 ]")
(dtk-define-voice 'paul-smooth
                  "[_: pi:1.1 fr:15500 ]")
(dtk-define-voice 'annotation-voice "[_: pi:1.1 fr:17000 ]")
(dtk-define-voice 'indent-voice  "[_: pi:1.2 vo:1.7 fr:17000 ]")
(dtk-define-voice 'paul-animated
                  "[_: pi:1 vo:2.5 fr:16600 ]")
(dtk-define-voice 'paul-monotone "[_: pi:1 fr:14500 ]")


;;}}}
;;{{{  Associate faces to standard voices:

(dtk-define-voice-alias 'bold 'paul-smooth)
(dtk-define-voice-alias 'bold-italic 'betty)
(dtk-define-voice-alias 'underline 'ursula)
(dtk-define-voice-alias 'fixed 'paul-monotone)
(dtk-define-voice-alias 'italic 'paul-animated)
(dtk-define-voice-alias 'excerpt 'annotation-voice )

;;}}}
;;{{{  Settings from Janet Cahn's thesis.

;;; the  following are taken from Janet Cahn's Masters thesis.
;;; I've modified them for the Dectalk Express.
;;; lo is g5 on express.
;;; Also get rid of absolute changes in speech rate.

(dtk-define-voice 'paul-angry 
                  "[_: pi:1 vo:2.5 fr:16300 ]")

(dtk-define-voice 
 'paul-disgusted 
 "[_: pi:0.9 vo:1.2 fr:15000 ]")

(dtk-define-voice 
 'paul-glad 
 "[_: pi:1.1 vo:2.1 fr:16100 ]")

(dtk-define-voice
 'paul-sad 
 "[_: pi:0.9 vo:1.4 fr:17000 ]")

(dtk-define-voice 'paul-scared 
                  "[_: pi:1.2 vo:0.9 fr:14000 ]")

(dtk-define-voice 'paul-surprised 
                  "[_: pi:1 vo:1.2 fr:15800 ]")

;;}}}
;;{{{  the inaudible voice

(dtk-define-voice 'inaudible "")

;;}}}
;;{{{  resetting the voice

(defvar dtk-voice-reset-code (dtk-get-voice-command dtk-default-voice
                                                    )
  "Memoize default voice command to make voice locking efficient.
This variable holds the code associated with dtk-default-voice
and saves repeated hash lookups.")

;;}}}
;;{{{  return list of all defined voices 

(defun voice-personality-list ()
  "Return list of all defined voices"
(declare (special dtk-voice-table))
(loop for key being the hash-keys of dtk-voice-table
      collect key ))

(setq tts-default-voice 'inaudible)

;;}}}
(provide 'dtk-voices)
;;{{{  emacs local variables 

;;; local variables:
;;; folded-file: t
;;; end: 

;;}}}
