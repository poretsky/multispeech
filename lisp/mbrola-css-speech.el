;;; $Id: mbrola-css-speech.el,v 1.1 2001/11/03 08:15:54 master Exp $
;;; $Author: master $ 
;;; Description: DTK Interface for Cascaded Speech Style Sheets
;;; Keywords:emacspeak, audio interface to emacs CSS 
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
;;; Copyright (c) 1996 by T. V. Raman 
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

(require 'cl)
(declaim  (optimize  (safety 0) (speed 3)))
(require 'dtk-voices)

;;{{{  Introduction:

;;; The CSS Speech Style Sheet specification defines a number of
;;; abstract device independent voice properties.
;;; A setting conforming to the CSS speech specification can be
;;; represented in elisp as a structure.

;;; We will refer to this structure as a "speech style".
;;; This structure needs to be mapped to device dependent codes to
;;; produce the desired effect.
;;; This module forms a bridge between User Agents e.g. Emacs-w3 that
;;; wish to implement a speech style sheet
;;; and Emacspeak's dtk-voices module.
;;; Emacspeak produces voice change effects by examining the value of
;;; text-property 'personality.
;;; Think of a buffer of formatted text along with the text-property
;;; 'personality appropriately set as a "aural display list".
;;; Applications like W3 that produce such formatted buffers  call function
;;; dtk-personality-from-speech-style  with a  "speech-style"
;;; --a structure as defined in this module and get back a symbol that
;;; they then assign to the value of property 'personality.
;;;Emacspeak's rendering engine then does the needful at the time
;;;speech is produced.
;;; Function dtk-personality-from-speech-style does the following:
;;; Takes as input a "speech style"
;;;(1)  Computes a symbol that will be used henceforth to refer to this
;;; specific speech style.
;;; (2) Examines emacspeak's internal voice table to see if this
;;; speech style has a voice already defined.
;;; If so it returns immediately.
;;; Otherwise, it does the additional work of defining a dtk-voice for
;;; future use.
;;; See module dtk-voices.el to see how voices are defined.

;;}}}
;;{{{  A speech style structure 

(defstruct  dtk-speech-style
  family
  gain left-volume right-volume
  average-pitch
  pitch-range
  stress
  richness
  )

;;}}}
;;{{{  Mapping css parameters to dtk codes

;;{{{ voice family codes 

(defvar dtk-family-table nil
  "Association list of dtk voice names and control codes")

(defsubst dtk-set-family-code (name code)
  (declare (special dtk-family-table))
  (when (stringp name)
    (setq name (intern name)))
  (setq dtk-family-table
        (cons (list name code )
              dtk-family-table)))

(defsubst dtk-get-family-code (name)
  (declare (special dtk-family-table ))
  (let ((sym nil))
  (when (stringp name)
    (setq name (intern name )))
  (or (cadr (assq  name dtk-family-table))
      "")))

(dtk-set-family-code 'paul "")
(dtk-set-family-code 'harry "")
(dtk-set-family-code 'dennis "")
(dtk-set-family-code 'frank "")
(dtk-set-family-code 'betty "")
(dtk-set-family-code 'ursula "")
(dtk-set-family-code 'wendy "")
(dtk-set-family-code 'rita "")
(dtk-set-family-code 'kid "")

;;}}}
;;{{{  hash table for mapping families to their dimensions

(defvar dtk-css-code-tables (make-hash-table)
  "Hash table holding vectors of dtk codes.
Keys are symbols of the form <FamilyName-Dimension>.
Values are vectors holding the control codes for the 10 settings.")

(defsubst dtk-css-set-code-table (family dimension table)
(declare (special dtk-css-code-tables))
(let ((key (intern (format "%s-%s" family dimension))))
(setf  (gethash key dtk-css-code-tables) table )))

(defsubst dtk-css-get-code-table (family dimension)
(declare (special dtk-css-code-tables))
(let ((key (intern (format "%s-%s" family dimension))))
(cl-gethash key dtk-css-code-tables)))

;;}}}
;;{{{ volume


(defvar dtk-gain-table (make-vector  10 "")
  "Maps CSS volume settings to actual synthesizer codes.")

;;}}}
;;{{{  average pitch

;;; Average pitch for standard male voice is 122hz --this is mapped to
;;; a setting of 5.
;;; Average pitch varies inversely with speaker head size --a child
;;; has a small head and a higher pitched voice.
;;; We change parameter head-size in conjunction with average pitch to
;;; produce a more natural change on the Dectalk.

;;{{{  paul average pitch

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format " pi:%s "
                  (second setting)))))
'(
(0 0.5)
(1 0.6)
(2 0.7)
(3 0.8)
(4 0.9 )
(5 1)
(6 1.1)
(7 1.2)
(8 1.3)
(9 1.4)
))
(dtk-css-set-code-table 'paul 'average-pitch table ))

;;}}}
;;{{{  harry average pitch

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format " pi:%s "
                  (second setting)))))
'(
(0 0.4)
(1 0.5)
(2 0.6)
(3 0.7)
(4 0.8 )
(5 0.9)
(6 1)
(7 1.1)
(8 1.2)
(9 1.3)
))
(dtk-css-set-code-table 'harry 'average-pitch table ))

;;}}}
;;{{{  betty average pitch

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format " pi:%s "
                  (second setting)))))
'(
(0 0.9)
(1 1)
(2 1.1)
(3 1.2)
(4 1.3 )
(5 1.4)
(6 1.5)
(7 1.6)
(8 1.7)
(9 1.8)
))
(dtk-css-set-code-table 'betty 'average-pitch table ))

;;}}}

(defsubst dtk-get-average-pitch-code (value family)
  (or family (setq family 'paul))
  (aref (dtk-css-get-code-table family 'average-pitch)
   value))

;;}}}
;;{{{  pitch range 

;;;  Standard pitch range is 100 and is  mapped to 
;;; a setting of 5.
;;; A value of 0 produces a flat monotone voice --maximum value of 250
;;; produces a highly animated voice.
;;; Additionally, we also set the assertiveness of the voice so the
;;; voice is less assertive at lower pitch ranges.
;;{{{  paul pitch range

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format " fr:%s "
                  (second setting)))))
'(
(0 15000)
(1 15200)
(2 15400)
(3 15600)
(4 15800 )
(5 16000 )
(6 16200)
(7 16400)
(8 16600)
(9 16800)
))
(dtk-css-set-code-table 'paul 'pitch-range table ))

;;}}}
;;{{{  harry pitch range

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format " fr:%s "
                  (second setting)))))
'(
(0 14000)
(1 14200)
(2 14400)
(3 14600)
(4 14800)
(5 15000)
(6 15200)
(7 15400)
(8 15600)
(9 15800)
))
(dtk-css-set-code-table 'harry 'pitch-range table ))

;;}}}
;;{{{  betty pitch range

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format " fr:%s "
                  (second setting)))))
'(
(0 17000)
(1 17200)
(2 17400)
(3 17600)
(4 17800 )
(5 18000)
(6 18200)
(7 18400)
(8 18600)
(9 18800)
))
(dtk-css-set-code-table 'betty 'pitch-range table ))

;;}}}
(defsubst dtk-get-pitch-range-code (value family)
  (or family (setq family 'paul))
  (aref (dtk-css-get-code-table family 'pitch-range)
  value))

;;}}}
;;{{{  stress

;;; On the Dectalk we vary four parameters
;;; The hat rise which controls the overall shape of the F0 contour
;;; for sentence level intonation and stress,
;;; The stress rise that controls the level of stress on stressed
;;; syllables,
;;; the baseline fall for paragraph level intonation 
;;; and the quickness --a parameter that controls whether the final
;;; frequency targets are completely achieved in the phonetic
;;; transitions.
;;{{{  paul stress 

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format ""))))
'(
(0  0 0 0 0)
(1 10 10 10 8)
(2 20 20 20 16)
(3 30 30 30 24 )
(4 40  40 40 32)
(5  50 50 50 40)
(6 60 60 60 40)
(7 70 70 70 40)
(8 80 80 80 40)
(9 90 90 90 40)
))
(dtk-css-set-code-table 'paul 'stress table))

;;}}}
;;{{{  harry stress 

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format ""))))
'(
(0  0 0 0 0)
(1 4 6 2 2 )
(2 8 12 4 4 )
(3 12 18 6 6 )
(4 16 24 8 8 )
(5 20 30 10 9)
(6 40  48 32 16)
(7 60 66 54 22)
(8 80 78 77 34)
(9 100 100 100 40)
))
(dtk-css-set-code-table 'harry 'stress table))

;;}}}
;;{{{  betty stress 

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table
          (first setting)
          (format ""))))
'(
(0  1 1 0 0)
(1 3 4 11 0)
(2 5 8 22 0)
(3 8 12 33 0 )
(4 11  16 44 0)
(5 14 20 55 0)
(6 35 40 65 10)
(7 56 80 75 20)
(8 77 90 85 30)
(9 100 100 100 40)
))
(dtk-css-set-code-table 'betty 'stress table))

;;}}}
(defsubst dtk-get-stress-code (value family)
  (or family (setq family 'paul ))
  (aref (dtk-css-get-code-table family 'stress)
        value))

;;}}}
;;{{{  richness

;;; Smoothness and richness vary inversely.
;;{{{  paul richness

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table (first setting)
          (format " ri:%s sm:%s "
                  (second setting)
                  (third setting)))))
'(
(0 0 100)
(1 14 80)
(2 28 60)
(3 42 40 )
(4 56 20)
(5 70  3 )
(6 60 24 )
(7 70 16)
(8 80 20)
(9 100  0)
))
(dtk-css-set-code-table 'paul 'richness table))

;;}}}
;;{{{  harry richness

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table (first setting)
          (format " ri:%s sm:%s "
                  (second setting)
                  (third setting)))))
'(
(0 100 0)
(1 96 3)
(2 93 6)
(3 90 9)
(4 88 11)
(5 86 12)
(6 60 24 )
(7 40 44)
(8 20 65)
(9 0 70)
))
(dtk-css-set-code-table 'harry 'richness table))

;;}}}
;;{{{  betty richness

(let ((table (make-vector 10 "")))
(mapcar
 (function
  (lambda (setting)
    (aset table (first setting)
          (format " ri:%s sm:%s "
                  (second setting)
                  (third setting)))))
'(
(0 0 100)
(1 8 76)
(2 16 52)
(3 24  28)
(4 32 10)
(5 40 4)
(6 50 3)
(7 65 3)
(8 80 2)
(9 100  0)
))
(dtk-css-set-code-table 'betty 'richness table))

;;}}}

(defsubst dtk-get-richness-code (value family)
  (or family (setq family 'paul))
  (aref (dtk-css-get-code-table family 'richness)
        value))

;;}}}

;;}}}
;;{{{  dtk-define-voice-from-speech-style

(defun dtk-define-voice-from-speech-style (name style)
  "Define name to be a dtk voice as specified by settings in style"
  (let* ((family(dtk-speech-style-family style))
          (command
         (concat "[_:"
                 (dtk-get-family-code family)
                 " "
(dtk-get-average-pitch-code (dtk-speech-style-average-pitch style) family)
(dtk-get-pitch-range-code (dtk-speech-style-pitch-range style) family)
(dtk-get-stress-code (dtk-speech-style-stress style ) family)
(dtk-get-richness-code (dtk-speech-style-richness style) family)
"]")))
(dtk-define-voice name command)))

;;}}}
;;{{{  dtk-personality-from-speech-style

(defun dectalk-personality-from-speech-style (style)
  "First compute a symbol that will be name for this style.
Then see if a voice defined for it.
Finally return the symbol"
  (cond
   ((= 0 (dtk-speech-style-gain style))
    'inaudible)
   (t 
    (let ((name (intern
                 (format "%s-%s%s%s%s"
                         (dtk-speech-style-family style)
                         (dtk-speech-style-average-pitch style)
                         (dtk-speech-style-pitch-range style)
                         (dtk-speech-style-stress style)
                         (dtk-speech-style-richness style)))))
      (unless (dtk-voice-defined-p name)
        (dtk-define-voice-from-speech-style name style))
      name))))

(fset 'dtk-personality-from-speech-style 'dectalk-personality-from-speech-style)

;;}}}
(provide  'dtk-css-speech)
;;{{{  emacs local variables 

;;; local variables:
;;; folded-file: t
;;; end: 

;;}}}
