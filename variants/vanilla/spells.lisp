;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: org.langband.vanilla -*-

#||

DESC: variants/vanilla/spells.lisp - spell-effects
Copyright (c) 2000-2003 - Stig Erik Sand�

This program is free software  ; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation	 ; either version 2 of the License, or
(at your option) any later version.

||#

(in-package :org.langband.vanilla)

(defun is-legal-effect-type? (effect-type)
  (find effect-type '(<teleport> "cold"> <light> "acid" "fire" <healing> <mana>
		      <divination> <enhance> <meteor>
		      "magic-missile" "poison" "electricity")
	:test #'equal))

;; hack
(defmacro spell-effect (arguments &body body)
  (assert (= (length arguments) 3))
  (let ((def `(lambda ,arguments
	       (declare (ignorable ,@arguments))
	       ,@body)))
;;    (warn "Def is ~s" def)
    `(function ,def)))

(defun define-spell-effect (id &key gfx-beam text-beam gfx-ball text-ball
			    gfx-orb text-orb gfx-bolts text-bolts)
  (declare (ignore gfx-beam text-beam))
  (assert (verify-id id))
  
  (let ((spell-effect (make-instance 'spell-effect :id id)))

    (when (arrayp gfx-bolts)
      (setf (projectile.gfx-path spell-effect) gfx-bolts))
    (when (arrayp text-bolts)
      (setf (projectile.text-path spell-effect) text-bolts))

    (when (numberp gfx-ball)
      (setf (projectile.gfx-explosion spell-effect) gfx-ball))
    (when (numberp text-ball)
      (setf (projectile.text-explosion spell-effect) text-ball))

    (when (numberp gfx-orb)
      (setf (projectile.gfx-impact spell-effect) gfx-orb))
    (when (numberp text-orb)
      (setf (projectile.text-impact spell-effect) text-orb))

    
    (setf (gethash id (variant.visual-effects *variant*)) spell-effect)
    
    spell-effect))

    
(defun get-spell-effect (type)
  #'(lambda (var source target &key x y damage state-object)
      (apply-spell-effect! var type source target :x x :y y :damage damage :state-object state-object)))

;;(defmethod print-mana-points ((variant vanilla-variant) player setting)
;;  (when (is-spellcaster? player)
;;    (call-next-method)))

(defmethod calculate-creature-mana! ((variant vanilla-variant) (player player))

  (unless (is-spellcaster? player)
    (setf (current-mana player) 0
	  (maximum-mana player) 0)
    (return-from calculate-creature-mana! nil))

  (let* ((old-mana (maximum-mana player))
	 (pl-class (player.class player))
	 (pl-lvl (player.level player))
	 (pl-eqp (player.equipment player))
	 (magic-level (+ 1 (- pl-lvl (class.spells-at-level pl-class))))
	 (stats (variant.stats variant))
	 (stat-obj (find (class.spell-stat pl-class) stats :key #'stat.symbol))
	 (new-mana 0))
    
    ;; no negative value
    (when (minusp magic-level)
      (setf magic-level 0))

    (when stat-obj
      (let* ((stat-val (aref (player.active-stats player) (stat.number stat-obj)))
	     (half-mana (get-stat-info stat-obj stat-val :half-mana)))

	;; hack
	(setf new-mana (int-/ (* half-mana magic-level) 2))
	(when (plusp new-mana)
	  (incf new-mana))

	))

    ;; check if gloves screw up mana
    (when (find '<cumbered-by-gloves> (class.abilities pl-class))
      (when-bind (gloves (item-table-find pl-eqp 'eq.glove))
	;;(warn "Wearing gloves ~s from mana ~s" gloves new-mana)
	;; add free action check
	(setf new-mana (int-/ (* new-mana 3) 4))
	))

    (let ((allowed-weight (class.max-armour-weight pl-class))
	  (weight 0))
      (unless (minusp allowed-weight) ;; anything allowed
	(dolist (i '(eq.armour eq.cloak eq.shield eq.head eq.glove eq.feet))
	  (when-bind (obj (item-table-find pl-eqp i))
	    (incf weight (object.weight obj))))

	(let ((factor (int-/ (- weight allowed-weight) 10)))
	  ;;(warn "Weight ~s and factor ~s" weight factor)
	  (when (plusp factor)
	    (decf new-mana factor)))

	))

    (when (minusp new-mana) ;; never negative
      (setf new-mana 0))
    
    (setf (maximum-mana player) new-mana)

    (when (> (current-mana player) new-mana)
      (setf (current-mana player) new-mana))
    
    (when (/= old-mana new-mana)
      (bit-flag-add! *redraw* +print-mana+))
    
    t))


(defun define-spell (name id &key effect-type effect numeric-id)
  "Defines and registers a new spell."

  (assert (stringp name))
  (assert (stringp id))

  (let ((variant *variant*)
	(spell (make-instance 'magic-spell :name name :id id)))

    (when (integerp numeric-id)
      (setf (spell.numeric-id spell) numeric-id))
    
    (when (and effect (functionp effect))
      (setf (spell.effect spell) (compile nil effect)))

    ;; checking carefully
    (when effect-type
      (unless (is-legal-effect-type? effect-type)
	(warn "Unknown spell-type ~s for spell ~s" effect-type id))

      (when-bind (lookup (gethash effect-type (variant.visual-effects variant)))
	;;(warn "spell lookup is ~s" lookup)
	(setf (spell.effect-type spell) lookup)))
    
    
    ;; register spell in variant
    (multiple-value-bind (value present-p)
	(gethash (spell.id spell) (variant.spells variant))
      (when present-p
	(warn "Replacing spell ~s in vanilla variant" value))
      (setf (gethash (spell.id spell) (variant.spells variant)) spell))
 
    
    spell))

(defmethod get-visual-projectile ((obj magic-spell))
  (spell.effect-type obj))

(defun create-spellbook (name id spells)
  "Creates and returns a spellbook."

  (check-type name string)
  (assert (verify-id id))
  (assert (consp spells))
	  
  (let* ((variant *variant*)
	 (len (length spells))
	 (book (make-instance 'spellbook :name name :id id :size len)))

    (setf (spellbook.spells book) (make-array len :initial-element nil))

    (loop for i from 0
	  for spell in spells
	  do
	  (let ((spell-obj (gethash spell (variant.spells variant))))
	    (cond ((and spell-obj (typep spell-obj 'magic-spell))
		   (setf (aref (spellbook.spells book) i) spell-obj))
		  (t
		   (warn "Unable to find spell ~s in vanilla-variant" spell))
		  )))
    book))

(defun register-spellbook& (variant book)
  "Registers spellbook in variant and returns the book."
  (multiple-value-bind (value present-p)
      (gethash (spellbook.id book) (variant.spellbooks variant))
    (when present-p
      (warn "Replacing spellbook ~s in vanilla variant" value))
    (setf (gethash (spellbook.id book) (variant.spellbooks variant)) book))
  
  book)

(defun define-spellbook (name id &key (size 6) (spells nil))
  "Defines and registers a spellbook, should be done after the spells have been
made."

  (declare (ignore size))
  (register-spellbook& *variant* (create-spellbook name id spells)))


(defun %simple-projection (source destination flag effect damage &key projected-object)
  "Destination can be a target or a direction.  A direction of 5 is interpreted
as target."
  
  (multiple-value-bind (dest-x dest-y)
      (%get-dest-coords source destination)
    (do-projection source dest-x dest-y flag :effect effect :damage damage :projected-object projected-object)))


(defun van-fire-beam! (player destination effect damage &key projected-object)
  "Fires a beam in a direction."
  (let ((flag (logior +project-kill+ +project-beam+ +project-through+)))
    (%simple-projection player destination flag effect damage :projected-object projected-object)))

(defun van-fire-bolt! (player destination effect damage &key projected-object)
  "Fires a bolt in a direction."
  (let ((flag (logior +project-kill+ +project-stop+ +project-through+)))
    (%simple-projection player destination flag effect damage :projected-object projected-object)))


(defun van-fire-bolt-or-beam! (player beam-chance destination effect damage &key projected-object)
  "Will fire a beam if beam-chance/100 happens, otherwise a bolt."
  (cond ((< (random 100) beam-chance)
	 (van-fire-beam! player destination effect damage :projected-object projected-object))
	(t
	 (van-fire-bolt! player destination effect damage :projected-object projected-object))))

(defun van-fire-ball! (source destination effect damage radius &key projected-object)
  "Fires a ball in a direction."
  (let ((flag (logior +project-kill+ +project-grid+ +project-stop+ +project-item+)))

    (multiple-value-bind (dest-x dest-y)
	(%get-dest-coords source destination 99)
      (do-projection source dest-x dest-y flag
		     :effect effect :damage damage
		     :radius radius
		     :projected-object projected-object)
      )))
  
  
(defun light-room! (dungeon x y &key (type '<light>))
  "Lights the room."
  (let ((coords (lb-ds:make-queue)))
    (flet ((add-coord (bx by)
	     (let ((flag (cave-flags dungeon bx by)))
	       ;; no recursion
	       ;;(warn "flag at ~s,~s is ~s" bx by flag)
	       (when (or (bit-flag-set? flag +cave-temp+)
			 ;; don't leave the room
			 (not (bit-flag-set? flag +cave-room+)))
		 (return-from add-coord))

	       (bit-flag-add! (cave-flags dungeon bx by) +cave-temp+)
	       ;;(warn "adding ~s ~s" bx by)
	       (lb-ds:enqueue (cons bx by) coords))))

      ;; add first grid
      (add-coord x y)

      (dolist (i (lb-ds:queue-as-list coords))
	(let ((cx (car i))
	      (cy (cdr i)))
	  (when (cave-floor-bold? dungeon cx cy)
	    ;; next to
	    (add-coord (1+ cx) cy)
	    (add-coord (1- cx) cy)
	    (add-coord cx (1+ cy))
	    (add-coord cx (1- cy))

	    ;; diagonal
	    (add-coord (1+ cx) (1+ cy))
	    (add-coord (1- cx) (1- cy))
	    (add-coord (1+ cx) (1- cy))
	    (add-coord (1- cx) (1+ cy))
	    ))))

    ;;(warn "coords ~s" coords)
    
    (dolist (i (lb-ds:queue-as-list coords))
      (let ((flag (cave-flags dungeon (car i) (cdr i))))
	(bit-flag-remove! flag +cave-temp+)
	;;(warn "lighting ~s ~s" (car i) (cdr i))
	(ecase type
	  (<light>
	   (bit-flag-add! flag +cave-glow+))
	  (<darkness>
	   (bit-flag-remove! flag +cave-glow+)))
	(setf (cave-flags dungeon (car i) (cdr i)) flag)))

    ;; redraw things
    (bit-flag-add! *update* +pl-upd-forget-view+ +pl-upd-update-view+)
    (bit-flag-add! *redraw* +print-map+)
    
    t))


(defun light-area! (dungeon player damage radius &key (type '<light>))
  "Lights the area."

  ;; unless blind
  (let ((blind-player nil)
	(px (location-x player))
	(py (location-y player)))
    
    (unless blind-player
      (ecase type
	(<light>
	 (print-message! "You are surrounded by a white light."))
	(<darkness>
	 (print-message! "Darkness surrounds you!"))
	))
    
    (do-projection player px py (logior +project-grid+ +project-kill+)
		   :damage damage
		   :radius radius
		   :effect (get-spell-effect type))
    (light-room! dungeon px py :type type))
  
  t)


(defun enchant-item! (dungeon player &key (type '<weapon>) (bonus 1) (restrict nil))

  (flet ((%local-enchant (item)
	   (let ((gvals (object.game-values item)))
	     (warn "enchant ~s ~s ~s ~s" item type bonus restrict)
	     ;; improve later
	     (ecase type
	       (<weapon>
		(when (< (gval.tohit-modifier gvals) +10)
		  (incf (gval.tohit-modifier gvals) bonus)
		  (incf (gval.dmg-modifier gvals) bonus)
		  :used))
	       
	       (<armour>
		(when (< (gval.ac-modifier gvals) +10)
		  (incf (gval.ac-modifier gvals) bonus)
		  :used))))))
  
    (let ((retval :still-useful)
	  (selection (select-item dungeon player '(:backpack :equip)
				  :prompt "Enchant item: "
				  :where :backpack)))

      (cond (selection
	     (let* ((the-table (get-item-table dungeon player (car selection)))
		    (removed-obj (item-table-remove! the-table (cdr selection))))
	       (cond (removed-obj
		      (format-message! "~a ~a glow~a brightly." "The" "[some-object, FIX]" "s")
		      (setf retval (%local-enchant removed-obj))
		    
		      (item-table-add! the-table removed-obj))
		     (t
		      (warn "Did not find selected obj ~a" selection)))))
	    (t
	     (warn "Did not select anything.")))
    

      retval)))

(defun get-spell-id (spell)
  (etypecase spell
    (string spell)
    (spell-classdata (spell.id spell))
    (magic-spell (spell.id spell))))
  
(defun get-spell-data (player spell)
  (when (is-spellcaster? player)
    (let ((spell-id (get-spell-id spell))
	  (spell-arr (class.spells (player.class player))))
      
      (when (vectorp spell-arr) ;; hack
	(loop for x across spell-arr
	      do
	      (when (equal spell-id (spell.id x))
		(return-from get-spell-data x))))
      
      nil)))


(defun learn-spell! (player spell)
  "Tries to ensure that the player learns the given spell."

  (unless (is-spellcaster? player)
    (print-message! "You are not a spellcaster and cannot learn spells.")
    (return-from learn-spell! nil))

;;  (warn "Trying to learn ~s" spell)
  
  (let ((spell-id (etypecase spell
		    (magic-spell (spell.id spell))
		    (spell-classdata (spell.id spell))
		    (string spell)))
	(learnt-spells (class.learnt-spells (player.class player))))

    (when (find spell-id learnt-spells :test #'equal)
      (print-message! "You already know the spell.")
      (return-from learn-spell! nil))

    (let ((spell-data (get-spell-data player spell-id)))
      (cond ((and (typep spell-data 'spell-classdata)
		  (<= (spell.level spell-data) (player.level player)))
	     (vector-push-extend spell-id learnt-spells)
	     (format-message! "~a learnt." (spell.name spell))
	     (bit-flag-add! *redraw* +print-study+)
	     (return-from learn-spell! t))
	    
	    ((and (typep spell-data 'spell-classdata)
		  (> (spell.level spell-data) (player.level player)))
	     (print-message! "You're not powerful enough to learn that spell yet."))
	    
	    ((eq spell-data nil)
	     (print-message! "You are unable to learn that spell."))
	    
	    (t
	     (warn "Unknown value returned ~s, ~s." spell-data spell)))

      nil)))

   
	
(defun has-learnt-spell? (player spell)
  "Returns NIL if the player has not learnt the spell,
returns T if the player knows the spell."
  (let* ((spell-id (get-spell-id spell))
	 (learnt-spells (class.learnt-spells (player.class player)))
	 (existing-spell (find spell-id learnt-spells :test #'equal)))
;;    (warn "Checked for ~s in ~s" spell-id learnt-spells)
    (when existing-spell
      t)))

(defun can-learn-more-spells? (variant player)
  "Returns T if the player can learn spells, NIL otherwise."
  (when (is-spellcaster? player)
    (let* ((pl-class (player.class player))
	   (stats (variant.stats variant))
	   (stat-obj (find (class.spell-stat pl-class) stats :key #'stat.symbol))
	   )
      (when stat-obj
	(let* ((stat-val (aref (player.active-stats player) (stat.number stat-obj)))
	       (half-spells (get-stat-info stat-obj stat-val :half-spells))
	       (learnt-spells (class.learnt-spells pl-class))
	       (num-learnt (length learnt-spells))
	       (max-spells (int-/ (* (player.level player) half-spells) 2)))
	  
	  
	  ;;(warn "Max spells ~s vs learnt ~s" max-spells num-learnt)

	  (> max-spells num-learnt)))
      )))

(defun interactive-book-selection (dungeon player)
  "Selects a book and returns it or NIL."
  (declare (ignore dungeon))
  (block select-book
    (let ((carrying (aobj.contains (player.inventory player)))
	  (books '())
	  ;;(variant *variant*)
	  )
    
      (item-table-iterate! carrying
			   #'(lambda (table num obj)
			       (declare (ignore table))
			       (when (typep obj 'active-object/book)
				 (push (cons obj num) books))))
      (setf books (nreverse books))

      (unless books
	(put-coloured-line! +term-white+ "No books" 0 0)
	(return-from select-book nil))
    
      (let* ((first-num (i2a (cdar books)))
	     (last-num (i2a (cdar (last books))))
	     (select-string (format nil "Inven: (~a-~a), * to see, ESC) Use which book? "
				    first-num last-num)))

	(put-coloured-line! +term-white+ select-string 0 0)
	(loop
	 (let ((selection (read-one-character)))
	   (cond ((eql selection +escape+)
		  (return-from select-book nil))
		 ((eql selection #\*)
		  (warn "Show book-selection not implemented."))
		 ((alpha-char-p selection)
		  (let ((num (a2i selection)))
		    (loop for (book . key) in books
			  do
			  (when (eql key num)
			    (return-from select-book book)))
		    (warn "Selection ~a not found, please try again." selection)
		    nil))
		 (t
		  (warn "Fell through with book-selection ~s" selection)
		  nil))
	   ))
      
	))))

(defun van-learn-spell! (dungeon player &key (variant *variant*))
  "Interactive selection of spell to learn."

  (unless (can-learn-more-spells? variant player)
    (print-message! "You cannot learn more spells at this level.")
    (return-from van-learn-spell! nil))
	   
  
  (block learn-spell
    (with-dialogue ()
      (when-bind (book (interactive-book-selection dungeon player))
	(let* ((okind (aobj.kind book))
	       (book-id (object.id okind)))
	  (when-bind (spell-info (gethash book-id (variant.spellbooks variant)))      
	    (when-bind (which-one (interactive-spell-selection player spell-info
							       :prompt "Learn which spell? "))
	      (unless (and (integerp which-one) (>= which-one 0)
			 (< which-one (spellbook.size spell-info)))
		(warn "Illegal choice ~s" which-one)
		(return-from learn-spell nil))
	      
	      (let ((the-spell (aref (spellbook.spells spell-info) which-one)))
		(learn-spell! player the-spell))

	      )))
	))
    ))


(defun van-invoke-spell! (dungeon player &key (variant *variant*))
  "Invokes a spell.. gee."

  (block cast-spell
    (when-bind (book (with-dialogue ()
		       (interactive-book-selection dungeon player)))
      (let* ((okind (aobj.kind book))
	     (book-id (object.id okind))
	     (which-one 0))
	(when-bind (spell-info (gethash book-id (variant.spellbooks variant)))
	  (with-dialogue ()
	    (setf which-one (interactive-spell-selection player spell-info)))

	  (cond ((eq which-one nil)
		 (return-from cast-spell nil))
		((not (and (integerp which-one) (>= which-one 0) (< which-one (spellbook.size spell-info))))
		 (warn "Spell ~s not found." which-one)
		 (return-from cast-spell nil)))

	  ;; let us find the spell now.
	  (let* ((the-spell (aref (spellbook.spells spell-info) which-one))
		 (spell-effect (spell.effect the-spell))
		 (spell-data (get-spell-data player the-spell))
		 (learnt-spell (has-learnt-spell? player the-spell)))

	    ;;(warn "Spell ~s: know (~s), learnt (~s)" the-spell spell-data learnt-spell)
	    
	    (unless (and spell-data learnt-spell)
	      (format-message! "You don't know the ~a spell." (spell.name the-spell))
	      (return-from cast-spell nil))

	    (unless (>= (current-mana player) (spell.mana spell-data))
	      (print-message! "You don't have enough mana to cast that spell.")
	      (return-from cast-spell nil))
	    
	    
	    (cond ((and spell-effect (functionp spell-effect))
		   (funcall spell-effect dungeon player the-spell)
		   ;; deduct mana, better way?
		   (decf (current-mana player) (spell.mana spell-data))
		   (bit-flag-add! *redraw* +print-mana+)
		   (unless (spell.tried spell-data)
		     ;;(warn "Tried spell ~s" (spell.id spell-data))
		     (setf (spell.tried spell-data) t)
		     (alter-xp! player (spell.xp spell-data)))

		   )
		  (t
		   (format-message! "The ~a spell is not implemented yet." (spell.name the-spell))))
	    ))

	;; clean up some!
	;; (put-coloured-line! +term-white+ "" 0 0)
	
      	)))
    
  (values))

(defun browse-spells (dungeon player &key (variant *variant*))
  "Interactive selection of spell to learn."

  (with-dialogue ()
    (when-bind (book (interactive-book-selection dungeon player))
      (let* ((okind (aobj.kind book))
	     (book-id (object.id okind)))
	(when-bind (spell-info (gethash book-id (variant.spellbooks variant)))
;;	  (warn "SI: ~s" spell-info)
	  (display-spells player spell-info)
	  (pause-last-line!)
	  )))))


(defun interactive-spell-selection (player spellbook &key (prompt "Cast which spell? "))
  "Returns selection."
  (block select-spell
    (loop
     (let ((select-string (format nil "(Spells ~a-~a, *=List, ESC) ~a"
				  #\a (i2a (1- (spellbook.size spellbook))) prompt)))

       (put-coloured-line! +term-white+ select-string 0 0)
       (let ((selection (read-one-character)))
	 (cond ((eql selection +escape+)
		(return-from select-spell nil))
	       ((eql selection #\*)
		(display-spells player spellbook)
		;; (warn "Display spellbook-spells not implemented.")
		nil)
	       ((alpha-char-p selection)
		(let ((num (a2i selection)))
		  (when (< num (spellbook.size spellbook))
		    (return-from select-spell num))))
	       (t
		(warn "Fell through spell-selection with choice: ~s" selection)))
	 )))
    ))


(defun display-spells (player spellbook &key (x 20) (y 1))
  "Displays the spell somehow."
  ;; l-blue for unknown
  ;; l-green for untried
  ;; white for normal/tried
  ;; red for difficult
  ;; l-dark for unreadable
  (let ((colour +term-white+)
	(comment "")
	(spells (spellbook.spells spellbook)))

    (put-coloured-line! +term-white+ "" x y)
    (put-coloured-line! +term-white+ "" x (1+ y))
    
    (put-coloured-str! +term-l-green+ (format nil "Book: ~a" (spellbook.name spellbook)) (+ x 8) y)
    (incf y)
    (put-coloured-str! +term-white+ "Name" (+ x 5) y)
    (put-coloured-str! +term-white+ "Lv Mana Fail Info" (+ x 35) y)
    
    (loop for i from 0
	  for spell across spells
	  do
	  (let ((spell-data (get-spell-data player spell))
		(row (+ y i 1)))

	    (cond ((not spell-data)
		   (put-coloured-line! +term-l-dark+ (format nil "  ~a) ~30a" (i2a i) "<unreadable>")
				       x row))
		  (t
		   (let ((base-level (spell.level spell-data))
			 (base-mana (spell.mana spell-data))
			 (base-fail (spell.failure spell-data))
			 (spell-tried (spell.tried spell-data))
			 (learnt-it (has-learnt-spell? player spell-data))
			 )
		     
		     ;; we have the spell readable at least
		     (cond ((< (player.level player) base-level)
			    (setf colour +term-red+
				  comment "difficult"))
			   
			   ((< (current-mana player) base-mana)
			    (setf colour +term-violet+)
			    (setf comment "low mana"))

			   ((not learnt-it)
			    (setf colour +term-l-blue+
				  comment "unknown"))
			   
			   ((not spell-tried)
			    (setf colour +term-l-green+
				  comment "untried"))
			   (t))
		   
		     (let ((str (format nil "  ~a) ~30a~2d ~4d ~3d% ~a"
					(i2a i) (spell.name spell) base-level base-mana base-fail comment
					)))
		       (put-coloured-line! colour str  x row))))
		  )))
    
    (put-coloured-line! +term-white+ "" x (+ (spellbook.size spellbook) y  1))
    
    nil
    ))
     

  
(defun %destroy-floor-obj (variant dungeon x y obj msg)
  (let ((item-table (cave-objects dungeon x y)) 
	(desc (with-output-to-string (s)
		(write-obj-description variant obj s))))
	   (format-message! "~a ~a." desc msg)
	   (item-table-remove! item-table obj)
	   (when (= 0 (items.cur-size item-table))
	     (setf (cave-objects dungeon x y) nil))
	   (light-spot! dungeon x y)))


(defmethod apply-spell-effect! ((variant vanilla-variant) type source target &key x y (damage 0) (state-object nil))
  (declare (ignore x y type damage source target))
  ;; do nothing default
;;  (warn "Fell through for ~s ~s" type target)
  state-object)


;;(defmethod apply-fire-effect! ((variant vanilla-variant) source target &key x y (damage 0) (state-object nil))
;;  (declare (ignore x y damage source target state-object))
;;  )


(defmethod apply-spell-effect! ((variant vanilla-variant) (type (eql '<fire>)) source (target active-object)
				&key x y (damage 0) (state-object nil))
  (declare (ignore source damage))
  (cond ((damaged-by-element? variant target '<fire>)
	 (%destroy-floor-obj variant *dungeon* x y target "burns"))
	(t
	 nil))
  state-object)

(defmethod apply-spell-effect! ((variant vanilla-variant) (type (eql '<magic-missile>)) source (target active-monster)
			       &key
			       x y (damage 0)  (state-object nil))
  (declare (ignore x y source damage))
  
  (when (meff.seen state-object)
    (setf (meff.obvious state-object) t))

  state-object)

    
  
(defmethod apply-spell-effect! ((variant vanilla-variant) (type (eql '<fire>)) source (target active-monster)
			       &key
			       x y (damage 0)  (state-object nil))
  (declare (ignore x y source))

  (when (meff.seen state-object)
    (setf (meff.obvious state-object) t))
  
  (unless (damaged-by-element? variant target '<fire>)
    ;; we're resisting
    (setf (meff.note state-object) " resists a lot.")
    (setf (meff.damage state-object) (int-/ damage 9))
    ;; skip lore
    )
    
  state-object)


(defun teleport-creature! (dungeon player creature range)
  (assert (numberp range))

  (let* ((minimum (floor range))
	 (cx (location-x creature))
	 (cy (location-y creature))
	 (tx cx)
	 (ty cy)
	 (cur-d range))
    (block find-grid
      (loop
       (when (> range 200)
	 (setf range 200))
       
       (block legal-dist
	 (dotimes (i 500)
	   (setf tx (rand-spread cx range)
		 ty (rand-spread cy range))
	   (setf cur-d (distance cx cy tx ty))
	   (when (and (>= cur-d minimum) (<= cur-d minimum))
	     (return-from legal-dist))))
       
       (when (and (in-bounds-fully? dungeon tx ty)
		  (can-place? dungeon tx ty :creature)
		  (not (cave-icky? dungeon tx ty)))
	 (return-from find-grid))
       
       (setf range (* 2 range)
	     minimum (floor minimum 2))))

    ;; we found an ok spot!
    (assert (and (in-bounds-fully? dungeon tx ty)
		 (can-place? dungeon tx ty :creature)
		 (not (cave-icky? dungeon tx ty))))

    ;; sound

    ;; swap monster
    (swap-monsters! dungeon player cx cy tx ty)
#||    
    (warn "UPD: ~s (~s ~s ~a)  -> (~s ~s ~a), ~s"
	  *update* cx cy (multiple-value-bind (a b) (map-info dungeon cx cy) b)
	  (location-x player) (location-y player)
    (multiple-value-bind (a b) (map-info dungeon (location-x player) (location-y player)) b)
	  (distance cx cy tx ty))
    ||#
;;    (handle-stuff dungeon player) ;; hack

;;    (print-map dungeon player)
    ))

(defun summon-monster (dungeon x y depth &key (type :any))
  "Returns T if it summoned a monster successfully."
  (declare (ignore type depth))
;;  (warn "summoning at (~s,~s) type ~s" x y type)

  ;; we ignore type now, and fix that later.

  (let ((variant *variant*)
	(player *player*)
	(retval t))
    (loop for i from 1 to 10
	  do
	  (let ((fx (+ (randint i) x))
		(fy (+ (randint i) y))) ;; hack
	    (when (cave-empty-bold? dungeon fx fy) 
	      (place-monster! variant dungeon player fx fy nil nil)
	      (setf retval t))
	    ))
    retval))


(defun detect-invisible! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  (let ((detected-any nil))
    (apply-effect-to-area dungeon
			  (- (location-x player) radius)
			  (- (location-y player) radius)
			  (* radius 2) (* radius 2)
			  #'(lambda (coord x y)
			      (declare (ignore x y))
			      (when-bind (monsters (coord.monsters coord))
				(dolist (i monsters)
				  (when (has-ability? (amon.kind i) '<invisible>)
				    (add-monster-knowledge-flag! player i '<invisible>)
				    (bit-flag-add! (amon.vis-flag i)
						   #.(logior +monster-flag-mark+ +monster-flag-show+))
				    (setf detected-any t)
				    (update-monster! *variant* i nil))))))
    (when detected-any
       (print-message! "You detect invisible creatures!"))
    
    detected-any))

(defun detect-evil-monsters! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  (let ((detected-any nil))
    (apply-effect-to-area dungeon
			  (- (location-x player) radius)
			  (- (location-y player) radius)
			  (* radius 2) (* radius 2)
			  #'(lambda (coord x y)
			      (declare (ignore x y))
			      (when-bind (monsters (coord.monsters coord))
				(dolist (i monsters)
				  (when (eq (monster.alignment (amon.kind i)) '<evil>)
				    (add-monster-knowledge-flag! player i '<evil>)
				    (bit-flag-add! (amon.vis-flag i)
						   #.(logior +monster-flag-mark+ +monster-flag-show+))
				    (setf detected-any t)
				    (update-monster! *variant* i nil))))))
    (when detected-any
       (print-message! "You sense the presence of evil creatures!"))
    
    detected-any))


(defun detect-monsters! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  
   (let ((detected-any nil))

     (apply-effect-to-area dungeon
			   (- (location-x player) radius)
			   (- (location-y player) radius)
			   (* radius 2) (* radius 2)
			   #'(lambda (coord x y)
			       (declare (ignore x y))
			       (when-bind (monsters (coord.monsters coord))
				 (dolist (i monsters)
				   (unless (has-ability? (amon.kind i) '<invisible>)
				     (bit-flag-add! (amon.vis-flag i)
						    #.(logior +monster-flag-mark+ +monster-flag-show+))
				     (setf detected-any t)
				     (update-monster! *variant* i nil))))))
     (when detected-any
       (print-message! "You sense the presence of monsters!"))

     detected-any))

(defun detect-traps! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  
   (let ((detected-any nil))

     (apply-effect-to-area dungeon
			   (- (location-x player) radius)
			   (- (location-y player) radius)
			   (* radius 2) (* radius 2)
			   #'(lambda (coord x y)
			       (when-bind (decor (coord.decor coord))
				 (when (is-trap? decor)
				   (bit-flag-add! (coord.flags coord) +cave-mark+)
				   (make-trap-visible! decor dungeon x y)
				   (setf detected-any t)))
			       ))
     
				 
     (when detected-any
       (print-message! "You sense the presence of traps!"))

     detected-any))

(defun detect-doors! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  
   (let ((detected-any nil))

     (apply-effect-to-area dungeon
			   (- (location-x player) radius)
			   (- (location-y player) radius)
			   (* radius 2) (* radius 2)
			   #'(lambda (coord x y)
			       (when-bind (decor (coord.decor coord))
				 (when (is-door? decor)
				   (bit-flag-add! (coord.flags coord) +cave-mark+)
				   (make-door-visible! decor dungeon x y)
				   (setf detected-any t)))
			       ))
     
				 
     (when detected-any
       (print-message! "You sense the presence of doors!"))

     detected-any))

(defun detect-stairs! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  
   (let ((detected-any nil))

     (apply-effect-to-area dungeon
			   (- (location-x player) radius)
			   (- (location-y player) radius)
			   (* radius 2) (* radius 2)
			   #'(lambda (coord x y)
			       (when-bind (floor (coord.floor coord))
				 (when (and (typep floor 'floor-type)
					    (or (= (floor.numeric-id floor) 77)
						(= (floor.numeric-id floor) 78)))
				   (bit-flag-add! (coord.flags coord) +cave-mark+)
				   (light-spot! dungeon x y)
				   (setf detected-any t)))
			       ))
     
				 
     (when detected-any
       (print-message! "You sense the presence of stairs!"))

     detected-any))


(defun detect-gold! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  
   (let ((detected-any nil))

     (apply-effect-to-area dungeon
			   (- (location-x player) radius)
			   (- (location-y player) radius)
			   (* radius 2) (* radius 2)
			   #'(lambda (coord x y)
			       (when-bind (objs (coord.objects coord))
				 (dolist (obj (items.objs objs))
				   (when (typep obj 'active-object/money)
				     (setf (aobj.marked obj) t)
				     ;;(bit-flag-add! (coord.flags coord) +cave-mark+)
				     (light-spot! dungeon x y)
				     (setf detected-any t)))
				 )))
     
				 
     (when detected-any
       (print-message! "You sense the presence of treasure!"))

     detected-any))

(defun detect-normal-objects! (dungeon player source &optional (radius +default-detect-radius+))
  (declare (ignore source))
  
   (let ((detected-any nil))

     (apply-effect-to-area dungeon
			   (- (location-x player) radius)
			   (- (location-y player) radius)
			   (* radius 2) (* radius 2)
			   #'(lambda (coord x y)
			       (when-bind (objs (coord.objects coord))
				 (dolist (obj (items.objs objs))
				   (unless (typep obj 'active-object/money)
				     (setf (aobj.marked obj) t)
				     ;;(bit-flag-add! (coord.flags coord) +cave-mark+)
				     (light-spot! dungeon x y)
				     (setf detected-any t)))
				 )))
     
				 
     (when detected-any
       (print-message! "You sense the presence of objects!"))

     detected-any))

(defun detect-all! (dungeon player source &optional (radius +default-detect-radius+))
  (detect-traps! dungeon player source radius)
  (detect-doors! dungeon player source radius)
  (detect-stairs! dungeon player source radius)
  (detect-gold! dungeon player source radius)
  (detect-normal-objects! dungeon player source radius)
  (detect-invisible! dungeon player source radius)
  (detect-monsters! dungeon player source radius)
  t)

;; FIX for type '<powerful>
(defun interactive-identify-object! (dungeon player &key (type '<normal>))

  (block id-obj
    (let* ((limit-from '(:backpack :floor :worn))
	   (prompt "Identify which item? ")
	   (variant *variant*)
	   (selection (select-item dungeon player limit-from
				   :prompt prompt
				   :where (first limit-from))))
    
      (unless (and selection (consp selection))
	(return-from id-obj nil))

      (let* ((the-table (get-item-table dungeon player (car selection)))
	     (removed-obj (item-table-remove! the-table (cdr selection))))
	  
	(unless (and removed-obj (typep removed-obj 'active-object))
	  (return-from id-obj nil))

	(ecase type
	  (<normal>
	   (possible-identify! player removed-obj))
	  (<powerful>
	   (possible-identify! player removed-obj)
	   (learn-about-object! player removed-obj :fully-known)))

	;; put object back where it was found
	(%put-obj-in-cnt dungeon player the-table removed-obj)

	(format-message! "Object is ~a."
			 (with-output-to-string (s)
			   (write-obj-description variant removed-obj s)))

	t))
    ))
      

(defmethod print-object ((inst magic-spell) stream)
  (print-unreadable-object
   (inst stream :identity t)
   (format stream "~:(~S~) [~S ~S]" (lbsys/class-name inst)
           (spell.id inst)
           (spell.name inst)))
  inst)

(defmethod print-object ((inst spellbook) stream)
  (print-unreadable-object
   (inst stream :identity t)
   (format stream "~:(~S~) [~S ~S]" (lbsys/class-name inst)
           (spellbook.id inst)
           (spellbook.name inst)))
  inst)

(defmethod print-object ((inst spell-classdata) stream)
  (print-unreadable-object
   (inst stream :identity t)
   (format stream "~:(~S~) [~S ~S]" (lbsys/class-name inst)
           (spell.id inst)
           (spell.level inst)))
  inst)



;;(trace light-area! light-room!)
#||
;; clash with above?
(defun has-spell? (player spell)
  "Returns NIL if the player cannot have the spell. Returns the spell-info if it does."
  (when (is-spellcaster? player)
    (let ((spells (class.spells (player.class player)))
	  (spell-id (spell.id spell)))
      (loop for x across spells
	    do
	    (when (equal (spell.id x) spell-id)
	      (return-from has-spell? x)))
      nil)))
||#
