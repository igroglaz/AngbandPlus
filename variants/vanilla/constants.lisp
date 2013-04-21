;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: org.langband.vanilla -*-

#|

DESC: variants/vanilla/constants.lisp - constants internal to vanilla
Copyright (c) 2002 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :org.langband.vanilla)

;;; === flags that control print/redraw

(defconstant +print-cut+    #x00001000)
(defconstant +print-stun+   #x00002000)
(defconstant +print-hunger+ #x00004000)

(defconstant +print-blind+    #x00010000)
(defconstant +print-confused+ #x00020000)
(defconstant +print-afraid+   #x00040000)
(defconstant +print-poisoned+ #x00080000)

(defconstant +print-study+    #x00400000)


(defconstant +default-detect-radius+ 25 "what is the radius of a detection-spell")
