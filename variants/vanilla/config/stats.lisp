;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: org.langband.vanilla -*-

#|

DESC: variants/vanilla/config/stats.lisp - stats for player characters
Copyright (c) 2002 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :org.langband.vanilla)

;;; Needs some reorganising

;; the format of a table is:
;; startnum endnum info-type info info-type info ...
;; where startnum is a positive integer
;; where endnum is a positive integer to name a range, nil when a single digit or 'rest when upwards
;; info-type is a keyword and info is the connected value

(define-character-stat '<str> "strength"
  :abbreviation "Str" :number 0 :positive-desc "strong" :negative-desc "weak"
  :data
  '((3 nil     :dam-modifier -2 :hit-modifier -3 :weight-limit 50  :wpn-limit 4   :dig-value   0 :blow-table   3)
    (4 nil     :dam-modifier -2 :hit-modifier -2 :weight-limit 60  :wpn-limit 5   :dig-value   0 :blow-table   4)
    (5 nil     :dam-modifier -1 :hit-modifier -1 :weight-limit 70  :wpn-limit 6   :dig-value   1 :blow-table   5)
    (6 nil     :dam-modifier -1 :hit-modifier -1 :weight-limit 80  :wpn-limit 7   :dig-value   2 :blow-table   6)
    (7 nil     :dam-modifier 0  :hit-modifier 0  :weight-limit 90  :wpn-limit 8   :dig-value   3 :blow-table   7)
    (8 nil     :dam-modifier 0  :hit-modifier 0  :weight-limit 100 :wpn-limit 10  :dig-value   4 :blow-table   8)
    (9 nil     :dam-modifier 0  :hit-modifier 0  :weight-limit 110 :wpn-limit 12  :dig-value   4 :blow-table   9)
    (10 nil    :dam-modifier 0  :hit-modifier 0  :weight-limit 120 :wpn-limit 14  :dig-value   5 :blow-table  10)
    (11 nil    :dam-modifier 0  :hit-modifier 0  :weight-limit 130 :wpn-limit 16  :dig-value   5 :blow-table  11)
    (12 nil    :dam-modifier 0  :hit-modifier 0  :weight-limit 140 :wpn-limit 18  :dig-value   6 :blow-table  12) 
    (13 nil    :dam-modifier 0  :hit-modifier 0  :weight-limit 150 :wpn-limit 20  :dig-value   6 :blow-table  13)
    (14 nil    :dam-modifier 0  :hit-modifier 0  :weight-limit 160 :wpn-limit 22  :dig-value   7 :blow-table  14)
    (15 nil    :dam-modifier 0  :hit-modifier 0  :weight-limit 170 :wpn-limit 24  :dig-value   7 :blow-table  15)
    (16 nil    :dam-modifier 1  :hit-modifier 0  :weight-limit 180 :wpn-limit 26  :dig-value   8 :blow-table  16)
    (17 nil    :dam-modifier 2  :hit-modifier 0  :weight-limit 190 :wpn-limit 28  :dig-value   8 :blow-table  17)
    (18 27     :dam-modifier 2  :hit-modifier 1  :weight-limit 200 :wpn-limit 30  :dig-value   9 :blow-table  20)
    (28 37     :dam-modifier 2  :hit-modifier 1  :weight-limit 220 :wpn-limit 30  :dig-value  10 :blow-table  30)
    (38 47     :dam-modifier 3  :hit-modifier 1  :weight-limit 240 :wpn-limit 35  :dig-value  12 :blow-table  40)
    (48 57     :dam-modifier 3  :hit-modifier 1  :weight-limit 260 :wpn-limit 40  :dig-value  15 :blow-table  50)
    (58 67     :dam-modifier 3  :hit-modifier 1  :weight-limit 280 :wpn-limit 45  :dig-value  20 :blow-table  60)
    (68 77     :dam-modifier 3  :hit-modifier 1  :weight-limit 300 :wpn-limit 50  :dig-value  25 :blow-table  70)
    (78 87     :dam-modifier 3  :hit-modifier 1  :weight-limit 300 :wpn-limit 55  :dig-value  30 :blow-table  80)
    (88 97     :dam-modifier 4  :hit-modifier 2  :weight-limit 300 :wpn-limit 60  :dig-value  35 :blow-table  90)
    (98 107    :dam-modifier 5  :hit-modifier 3  :weight-limit 300 :wpn-limit 65  :dig-value  40 :blow-table 100)
    (108 117   :dam-modifier 5  :hit-modifier 4  :weight-limit 300 :wpn-limit 70  :dig-value  45 :blow-table 110)
    (118 127   :dam-modifier 6  :hit-modifier 5  :weight-limit 300 :wpn-limit 80  :dig-value  50 :blow-table 120)
    (128 137   :dam-modifier 7  :hit-modifier 6  :weight-limit 300 :wpn-limit 80  :dig-value  55 :blow-table 130)
    (138 147   :dam-modifier 8  :hit-modifier 7  :weight-limit 300 :wpn-limit 80  :dig-value  60 :blow-table 140)
    (148 157   :dam-modifier 9  :hit-modifier 8  :weight-limit 300 :wpn-limit 80  :dig-value  65 :blow-table 150)
    (158 167   :dam-modifier 10 :hit-modifier 9  :weight-limit 300 :wpn-limit 80  :dig-value  70 :blow-table 160)
    (168 177   :dam-modifier 11 :hit-modifier 10 :weight-limit 300 :wpn-limit 90  :dig-value  75 :blow-table 170)
    (178 187   :dam-modifier 12 :hit-modifier 11 :weight-limit 300 :wpn-limit 90  :dig-value  80 :blow-table 180)
    (188 197   :dam-modifier 13 :hit-modifier 12 :weight-limit 300 :wpn-limit 90  :dig-value  85 :blow-table 190)
    (198 207   :dam-modifier 14 :hit-modifier 13 :weight-limit 300 :wpn-limit 90  :dig-value  90 :blow-table 200)
    (208 217   :dam-modifier 15 :hit-modifier 14 :weight-limit 300 :wpn-limit 90  :dig-value  95 :blow-table 210)
    (218 227   :dam-modifier 16 :hit-modifier 15 :weight-limit 300 :wpn-limit 100 :dig-value 100 :blow-table 220) 
    (228 237   :dam-modifier 18 :hit-modifier 15 :weight-limit 300 :wpn-limit 100 :dig-value 100 :blow-table 230)
    (238 'rest :dam-modifier 20 :hit-modifier 15 :weight-limit 300 :wpn-limit 100 :dig-value 100 :blow-table 240)
    ))


(define-character-stat '<dex> "dexterity" :positive-desc "dextrous" :negative-desc "clumsy"
  :abbreviation "Dex" :number 1
  :data
  '((3 nil     :ac-modifier -4 :hit-modifier -3 :disarm 0  :evasion 0    :blow-table  0)
    (4 nil     :ac-modifier -3 :hit-modifier -2 :disarm 0  :evasion 1    :blow-table  0)
    (5 nil     :ac-modifier -2 :hit-modifier -2 :disarm 0  :evasion 2    :blow-table  0)
    (6 nil     :ac-modifier -1 :hit-modifier -1 :disarm 0  :evasion 3    :blow-table  0)
    (7 nil     :ac-modifier 0  :hit-modifier -1 :disarm 0  :evasion 4    :blow-table  0)
    (8 nil     :ac-modifier 0  :hit-modifier 0  :disarm 0  :evasion 5    :blow-table  0)
    (9 nil     :ac-modifier 0  :hit-modifier 0  :disarm 0  :evasion 5    :blow-table  0)
    (10 nil    :ac-modifier 0  :hit-modifier 0  :disarm 0  :evasion 6    :blow-table  1)
    (11 nil    :ac-modifier 0  :hit-modifier 0  :disarm 0  :evasion 6    :blow-table  1)
    (12 nil    :ac-modifier 0  :hit-modifier 0  :disarm 0  :evasion 7    :blow-table  1) 
    (13 nil    :ac-modifier 0  :hit-modifier 0  :disarm 1  :evasion 7    :blow-table  1)
    (14 nil    :ac-modifier 0  :hit-modifier 0  :disarm 1  :evasion 8    :blow-table  1)
    (15 nil    :ac-modifier 1  :hit-modifier 0  :disarm 1  :evasion 8    :blow-table  1)
    (16 nil    :ac-modifier 1  :hit-modifier 1  :disarm 2  :evasion 9    :blow-table  1)
    (17 nil    :ac-modifier 1  :hit-modifier 2  :disarm 2  :evasion 9    :blow-table  1)
    (18 27     :ac-modifier 2  :hit-modifier 3  :disarm 4  :evasion 10   :blow-table  1)
    (28 37     :ac-modifier 2  :hit-modifier 3  :disarm 4  :evasion 10   :blow-table  2)
    (38 47     :ac-modifier 2  :hit-modifier 3  :disarm 4  :evasion 15   :blow-table  2)
    (48 57     :ac-modifier 2  :hit-modifier 3  :disarm 4  :evasion 15   :blow-table  2)
    (58 67     :ac-modifier 2  :hit-modifier 3  :disarm 5  :evasion 20   :blow-table  2)
    (68 77     :ac-modifier 3  :hit-modifier 4  :disarm 5  :evasion 25   :blow-table  3)
    (78 87     :ac-modifier 3  :hit-modifier 4  :disarm 5  :evasion 30   :blow-table  3)
    (88 97     :ac-modifier 3  :hit-modifier 4  :disarm 6  :evasion 35   :blow-table  4)
    (98 107    :ac-modifier 4  :hit-modifier 4  :disarm 6  :evasion 40   :blow-table  4)
    (108 117   :ac-modifier 5  :hit-modifier 5  :disarm 7  :evasion 45   :blow-table  5)
    (118 127   :ac-modifier 6  :hit-modifier 6  :disarm 8  :evasion 50   :blow-table  6)
    (128 137   :ac-modifier 7  :hit-modifier 7  :disarm 8  :evasion 60   :blow-table  7)
    (138 147   :ac-modifier 8  :hit-modifier 8  :disarm 8  :evasion 70   :blow-table  8)
    (148 157   :ac-modifier 9  :hit-modifier 9  :disarm 8  :evasion 80   :blow-table  9)
    (158 167   :ac-modifier 9  :hit-modifier 9  :disarm 8  :evasion 90   :blow-table 10)
    (168 177   :ac-modifier 10 :hit-modifier 10 :disarm 9  :evasion 100  :blow-table 11)
    (178 187   :ac-modifier 11 :hit-modifier 11 :disarm 9  :evasion 100  :blow-table 12)
    (188 197   :ac-modifier 12 :hit-modifier 12 :disarm 9  :evasion 100  :blow-table 14)
    (198 207   :ac-modifier 13 :hit-modifier 13 :disarm 9  :evasion 100  :blow-table 16)
    (208 217   :ac-modifier 14 :hit-modifier 14 :disarm 9  :evasion 100  :blow-table 18)
    (218 227   :ac-modifier 15 :hit-modifier 15 :disarm 10 :evasion 100  :blow-table 20) 
    (228 237   :ac-modifier 15 :hit-modifier 15 :disarm 10 :evasion 100  :blow-table 20)
    (238 'rest :ac-modifier 15 :hit-modifier 15 :disarm 10 :evasion 100  :blow-table 20)
    ))

(define-character-stat '<con> "constitution"
  :abbreviation "Con" :number 2 :positive-desc "healthy" :negative-desc "sickly"
  :data
  '((3 nil     :regeneration 0 :half-hp -5)
    (4 nil     :regeneration 0 :half-hp -3)
    (5 nil     :regeneration 0 :half-hp -2)
    (6 nil     :regeneration 0 :half-hp -1)
    (7 nil     :regeneration 0 :half-hp 0 )
    (8 nil     :regeneration 0 :half-hp 0 )
    (9 nil     :regeneration 0 :half-hp 0 )
    (10 nil    :regeneration 0 :half-hp 0 )
    (11 nil    :regeneration 0 :half-hp 0 )
    (12 nil    :regeneration 0 :half-hp 0 )
    (13 nil    :regeneration 0 :half-hp 0 )
    (14 nil    :regeneration 1 :half-hp 0 )
    (15 nil    :regeneration 1 :half-hp 1 )
    (16 nil    :regeneration 1 :half-hp 1 )
    (17 nil    :regeneration 1 :half-hp 2 )
    (18 27     :regeneration 2 :half-hp 3 )
    (28 37     :regeneration 2 :half-hp 4 )
    (38 47     :regeneration 2 :half-hp 4 )
    (48 57     :regeneration 2 :half-hp 4 )
    (58 67     :regeneration 2 :half-hp 4 )
    (68 77     :regeneration 3 :half-hp 5 )
    (78 87     :regeneration 3 :half-hp 6 )
    (88 97     :regeneration 3 :half-hp 7 )
    (98 107    :regeneration 3 :half-hp 8 )
    (108 117   :regeneration 3 :half-hp 9 )
    (118 127   :regeneration 4 :half-hp 10)
    (128 137   :regeneration 4 :half-hp 11)
    (138 147   :regeneration 5 :half-hp 12)
    (148 157   :regeneration 6 :half-hp 13)
    (158 167   :regeneration 6 :half-hp 14)
    (168 177   :regeneration 7 :half-hp 15)
    (178 187   :regeneration 7 :half-hp 16)
    (188 197   :regeneration 8 :half-hp 18)
    (198 207   :regeneration 8 :half-hp 20)
    (208 217   :regeneration 8 :half-hp 22)
    (218 227   :regeneration 9 :half-hp 25) 
    (228 237   :regeneration 9 :half-hp 25)
    (238 'rest :regeneration 9 :half-hp 25)
    ))

(define-character-stat '<int> "intelligence"
  :abbreviation "Int" :number 3 :positive-desc "smart" :negative-desc "stupid"
  :data
  '((3 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :mag-dev 0  :disarm 0)
    (4 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :mag-dev 0  :disarm 0)
    (5 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :mag-dev 0  :disarm 0)
    (6 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :mag-dev 0  :disarm 0)
    (7 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :mag-dev 0  :disarm 0)
    (8 nil     :half-spells 1 :half-mana 1  :min-fail 50 :various 1  :mag-dev 1  :disarm 1)
    (9 nil     :half-spells 1 :half-mana 2  :min-fail 30 :various 1  :mag-dev 1  :disarm 1)
    (10 nil    :half-spells 1 :half-mana 2  :min-fail 20 :various 1  :mag-dev 1  :disarm 1)
    (11 nil    :half-spells 1 :half-mana 2  :min-fail 15 :various 1  :mag-dev 1  :disarm 1)
    (12 nil    :half-spells 2 :half-mana 2  :min-fail 12 :various 1  :mag-dev 1  :disarm 1)
    (13 nil    :half-spells 2 :half-mana 2  :min-fail 11 :various 1  :mag-dev 1  :disarm 1)
    (14 nil    :half-spells 2 :half-mana 2  :min-fail 10 :various 1  :mag-dev 1  :disarm 1)
    (15 nil    :half-spells 2 :half-mana 2  :min-fail 9  :various 2  :mag-dev 2  :disarm 2)
    (16 nil    :half-spells 2 :half-mana 2  :min-fail 8  :various 2  :mag-dev 2  :disarm 2)
    (17 nil    :half-spells 2 :half-mana 2  :min-fail 7  :various 2  :mag-dev 2  :disarm 2)
    (18 27     :half-spells 2 :half-mana 3  :min-fail 6  :various 3  :mag-dev 3  :disarm 3)
    (28 37     :half-spells 2 :half-mana 3  :min-fail 6  :various 3  :mag-dev 3  :disarm 3)
    (38 47     :half-spells 2 :half-mana 3  :min-fail 5  :various 3  :mag-dev 4  :disarm 3)
    (48 57     :half-spells 2 :half-mana 3  :min-fail 5  :various 3  :mag-dev 4  :disarm 4)
    (58 67     :half-spells 2 :half-mana 3  :min-fail 5  :various 3  :mag-dev 5  :disarm 4)
    (68 77     :half-spells 3 :half-mana 4  :min-fail 4  :various 4  :mag-dev 5  :disarm 5)
    (78 87     :half-spells 3 :half-mana 4  :min-fail 4  :various 4  :mag-dev 6  :disarm 6)
    (88 97     :half-spells 3 :half-mana 5  :min-fail 4  :various 5  :mag-dev 6  :disarm 7)
    (98 107    :half-spells 3 :half-mana 6  :min-fail 4  :various 6  :mag-dev 7  :disarm 8)
    (108 117   :half-spells 4 :half-mana 7  :min-fail 3  :various 7  :mag-dev 7  :disarm 9)
    (118 127   :half-spells 4 :half-mana 8  :min-fail 3  :various 8  :mag-dev 8  :disarm 10)
    (128 137   :half-spells 4 :half-mana 9  :min-fail 2  :various 9  :mag-dev 9  :disarm 10)
    (138 147   :half-spells 5 :half-mana 10 :min-fail 2  :various 10 :mag-dev 10 :disarm 11)
    (148 157   :half-spells 5 :half-mana 11 :min-fail 2  :various 11 :mag-dev 11 :disarm 12)
    (158 167   :half-spells 5 :half-mana 12 :min-fail 2  :various 12 :mag-dev 12 :disarm 13)
    (168 177   :half-spells 5 :half-mana 13 :min-fail 1  :various 13 :mag-dev 13 :disarm 14)
    (178 187   :half-spells 5 :half-mana 14 :min-fail 1  :various 14 :mag-dev 14 :disarm 15)
    (188 197   :half-spells 5 :half-mana 15 :min-fail 1  :various 15 :mag-dev 15 :disarm 16)
    (198 207   :half-spells 5 :half-mana 16 :min-fail 1  :various 16 :mag-dev 16 :disarm 17)
    (208 217   :half-spells 5 :half-mana 16 :min-fail 1  :various 17 :mag-dev 17 :disarm 18)
    (218 227   :half-spells 5 :half-mana 16 :min-fail 0  :various 18 :mag-dev 18 :disarm 19)
    (228 237   :half-spells 5 :half-mana 16 :min-fail 0  :various 19 :mag-dev 19 :disarm 19)
    (238 'rest :half-spells 5 :half-mana 16 :min-fail 0  :various 20 :mag-dev 20 :disarm 19)
    ))

(define-character-stat '<wis> "wisdom"
  :abbreviation "Wis" :number 4 :positive-desc "wise" :negative-desc "na�ve"
  :data
  '((3 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :saving-throw 0)
    (4 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :saving-throw 0)
    (5 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :saving-throw 0)
    (6 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :saving-throw 0)
    (7 nil     :half-spells 0 :half-mana 0  :min-fail 99 :various 0  :saving-throw 0)
    (8 nil     :half-spells 1 :half-mana 1  :min-fail 50 :various 1  :saving-throw 1)
    (9 nil     :half-spells 1 :half-mana 2  :min-fail 30 :various 1  :saving-throw 1)
    (10 nil    :half-spells 1 :half-mana 2  :min-fail 20 :various 1  :saving-throw 1)
    (11 nil    :half-spells 1 :half-mana 2  :min-fail 15 :various 1  :saving-throw 1)
    (12 nil    :half-spells 2 :half-mana 2  :min-fail 12 :various 1  :saving-throw 1)
    (13 nil    :half-spells 2 :half-mana 2  :min-fail 11 :various 1  :saving-throw 1)
    (14 nil    :half-spells 2 :half-mana 2  :min-fail 10 :various 1  :saving-throw 1)
    (15 nil    :half-spells 2 :half-mana 2  :min-fail 9  :various 2  :saving-throw 2)
    (16 nil    :half-spells 2 :half-mana 2  :min-fail 8  :various 2  :saving-throw 2)
    (17 nil    :half-spells 2 :half-mana 2  :min-fail 7  :various 2  :saving-throw 2)
    (18 27     :half-spells 2 :half-mana 3  :min-fail 6  :various 3  :saving-throw 3)
    (28 37     :half-spells 2 :half-mana 3  :min-fail 6  :various 3  :saving-throw 3)
    (38 47     :half-spells 2 :half-mana 3  :min-fail 5  :various 3  :saving-throw 3)
    (48 57     :half-spells 2 :half-mana 3  :min-fail 5  :various 3  :saving-throw 3)
    (58 67     :half-spells 2 :half-mana 3  :min-fail 5  :various 3  :saving-throw 3)
    (68 77     :half-spells 3 :half-mana 4  :min-fail 4  :various 4  :saving-throw 4)
    (78 87     :half-spells 3 :half-mana 4  :min-fail 4  :various 4  :saving-throw 4)
    (88 97     :half-spells 3 :half-mana 5  :min-fail 4  :various 5  :saving-throw 5)
    (98 107    :half-spells 3 :half-mana 6  :min-fail 4  :various 6  :saving-throw 5)
    (108 117   :half-spells 4 :half-mana 7  :min-fail 3  :various 7  :saving-throw 6)
    (118 127   :half-spells 4 :half-mana 8  :min-fail 3  :various 8  :saving-throw 7)
    (128 137   :half-spells 4 :half-mana 9  :min-fail 2  :various 9  :saving-throw 8)
    (138 147   :half-spells 5 :half-mana 10 :min-fail 2  :various 10 :saving-throw 9)
    (148 157   :half-spells 5 :half-mana 11 :min-fail 2  :various 11 :saving-throw 10)
    (158 167   :half-spells 5 :half-mana 12 :min-fail 2  :various 12 :saving-throw 11)
    (168 177   :half-spells 5 :half-mana 13 :min-fail 1  :various 13 :saving-throw 12)
    (178 187   :half-spells 5 :half-mana 14 :min-fail 1  :various 14 :saving-throw 13)
    (188 197   :half-spells 5 :half-mana 15 :min-fail 1  :various 15 :saving-throw 14)
    (198 207   :half-spells 5 :half-mana 16 :min-fail 1  :various 16 :saving-throw 15)
    (208 217   :half-spells 5 :half-mana 16 :min-fail 1  :various 17 :saving-throw 16)
    (218 227   :half-spells 5 :half-mana 16 :min-fail 0  :various 18 :saving-throw 17)
    (228 237   :half-spells 5 :half-mana 16 :min-fail 0  :various 19 :saving-throw 18)
    (238 'rest :half-spells 5 :half-mana 16 :min-fail 0  :various 20 :saving-throw 19)
    ))


;; payment in stores
(define-character-stat '<chr> "charisma"
  :abbreviation "Chr" :number 5 :positive-desc "cute" :negative-desc "ugly"
  :data
  '((3 nil     :payment 130)
    (4 nil     :payment 125)
    (5 nil     :payment 122)
    (6 nil     :payment 120)
    (7 nil     :payment 118)
    (8 nil     :payment 116)
    (9 nil     :payment 114)
    (10 nil    :payment 112)
    (11 nil    :payment 110)
    (12 nil    :payment 108)
    (13 nil    :payment 106)
    (14 nil    :payment 104)
    (15 nil    :payment 103)
    (16 nil    :payment 102)
    (17 nil    :payment 101)
    (18 27     :payment 100)
    (28 37     :payment  99)
    (38 47     :payment  98)
    (48 57     :payment  97)
    (58 67     :payment  96)
    (68 77     :payment  95)
    (78 87     :payment  94)
    (88 97     :payment  93)
    (98 107    :payment  92)
    (108 117   :payment  91)
    (118 127   :payment  90)
    (128 137   :payment  89)
    (138 147   :payment  88)
    (148 157   :payment  87)
    (158 167   :payment  86)
    (168 177   :payment  85)
    (178 187   :payment  84)
    (188 197   :payment  83)
    (198 207   :payment  82)
    (208 217   :payment  81)
    (218 227   :payment  80) 
    (228 237   :payment  80)
    (238 'rest :payment  80)
    ))

