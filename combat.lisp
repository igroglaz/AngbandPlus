;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: org.langband.engine -*-

#|

DESC: combat.lisp - the combat-system
Copyright (c) 2000-2002 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :org.langband.engine)

(defun check-for-hits ()

  )

(defmethod kill-target! (dungeon attacker (target active-monster) x y)
  "Tries to remove the monster at the location."

  (declare (ignore attacker))
  
  (let ((var-obj *variant*)
	(lvl-obj *level*)
	(the-kind (amon.kind target)))
    
    (setf (creature-alive? target) nil)
    
    ;; let us generate drop.. 
    (when (can-creature-drop? var-obj target)
      (creature-drop! var-obj target lvl-obj))
    
    (setf (dungeon.monsters dungeon) (delete target (dungeon.monsters dungeon))
	  (cave-monsters dungeon x y) nil)

    ;; hack
    (when (typep the-kind 'unique-monster)
      (setf (monster.already-dead the-kind) t))
    
    
    t))
      


  
(defmethod kill-target! (dungeon attacker (target player) x y)
  "Tries to remove the monster at the location."
  (declare (ignore dungeon x y))

  (let ((killer (cond ((stringp attacker)
		       attacker)
		      ;; unsafe, but will give errors
		      (t
		       (get-creature-name attacker)))))

  
    (setf (creature-alive? target) nil)
    (setf (player.dead-from target) killer)
  
  nil))

(defmethod cmb-describe-miss (attacker target)

  ;; update with uniques later
  (let ((p-or-u? (is-player? target)))
  
    (with-foreign-str (s)
      (lb-format s "~a misses ~a~a."
		 (get-creature-name attacker)
		 (if p-or-u? "" "the ")
		 (get-creature-name target))
      (print-message! s)))
  nil)


(defun %calc-perchance (chance the-ac)
  (let ((mid-res (int-/ (* 90 (- chance (int-/ (* 3 the-ac) 4))) chance)))
;;    (warn "mid-res is ~a" mid-res)
    (+ 5 mid-res)))

;;(trace %calc-perchance)

(defun melee-hit-ac? (the-target combat-skill the-ac visible-p)
  "Helper-function that checks if something was a hit."
  (declare (ignore the-target))
  
  (let ((k (random 100)))
    ;; instant miss or hit (5% chance each)
    (when (< k 10) (return-from melee-hit-ac? (< k 5))))
  
  (unless visible-p
    (setq combat-skill (int-/ combat-skill 2)))

  #+chance-warning
  (warn "Chance to hit '~a' with skill ~s vs AC ~a is ~a%"
	(get-creature-name the-target) combat-skill the-ac
	(%calc-perchance combat-skill the-ac))
  
  ;; this check is not 100% correct, but it does the job fairly accurately
  (if (and (plusp combat-skill)
	   (>= (random combat-skill)
	       (int-/ (* 3 the-ac)
		      4)))
      t
      nil))

	    
(defmethod melee-hit-creature? ((attacker player) (target active-monster) the-attack)
  (declare (ignore the-attack))
 
    (let* ((bonus 0) ;; (* (+ to-hit for weapon and dex/str) multiplier)
	   (skills (player.skills attacker))
	   (chance (+ (skills.fighting skills) bonus))
	   (monster-ac (get-creature-ac target))
	   (visible-p t))
      
    (melee-hit-ac? target chance monster-ac visible-p)))


(defmethod melee-hit-creature? ((attacker active-monster) (target player) the-attack)
  (declare (ignore the-attack))
    
  (let* ((power 60) ;; hit_hurt
	 (mlvl (monster.depth (amon.kind attacker)))
	 (rlev (if (plusp mlvl) mlvl 1)))
    
    (melee-hit-ac? target (+ power (* 3 rlev))
		 (get-creature-ac target)
		 t)))


(defun deduct-hp! (target amount)
  (decf (current-hp target) amount))

(defmethod melee-inflict-damage! ((attacker active-monster) target the-attack)
  
  (let (;;(kind (amon.kind attacker))
	(dmg-dice (attack.damage the-attack)))

    (cond ((not (consp dmg-dice))
	   0)

	  (t
	   (let ((dmg (roll-dice (car dmg-dice) (cdr dmg-dice))))
;;	     (warn "~ad~a gave ~a dmg to attacker (~a -> ~a hps)"
;;		   (car dmg-dice) (cdr dmg-dice) dmg
;;		   (current-hp target) (- (current-hp target) dmg))
	     (deduct-hp! target dmg)
	     (when (is-player? target)
	       (bit-flag-add! *redraw* +print-hp+))

	     dmg))
	  )))


  

(defun get-attk-desc (the-attack)
  (let* ((descs (variant.attk-descs *variant*))
	 (desc (gethash (attack.kind the-attack) descs)))
    (if desc
	desc
	"hits you.")))

(defun add-attk-desc (var-obj key desc)
  (setf (gethash key (variant.attk-descs var-obj)) desc))

(defmethod cmb-describe-hit ((attacker active-monster) (target player) the-attack)
  (let ((desc (get-attk-desc the-attack)))
    (with-foreign-str (s)
      (lb-format s "The ~a ~a " (get-creature-name attacker) desc)
      (print-message! s))))

(defmethod cmb-describe-hit (attacker target the-attack)
  (declare (ignore the-attack))
  (with-foreign-str (s)
    (lb-format s "~a hits the ~a. "
	    (get-creature-name attacker)
	    (get-creature-name target))
;;    (warn "Going format on ~s" (type-of s))
    (print-message! s))
  nil)

(defmethod cmb-describe-death (attacker target)
  (declare (ignore attacker))
  (with-foreign-str (s)
    (lb-format s "The ~a dies.. " (get-creature-name target))
    (print-message! s))
  nil)

(defun attack-target! (dungeon attacker target x y the-attack)
  (play-sound 1)
  ;;	(describe the-monster)
  (if (not (melee-hit-creature? attacker target the-attack))
      (cmb-describe-miss attacker target)
      (progn
	(cmb-describe-hit attacker target the-attack)
	(melee-inflict-damage! attacker target the-attack)
	      	      
	(when (< (current-hp target) 0)
	  (cmb-describe-death attacker target)
	  (let ((target-xp (get-xp-value target)))
	    (alter-xp! attacker (if target-xp target-xp 0)))
		
	  (kill-target! dungeon attacker target x y)
	  ;; repaint
	  (light-spot! dungeon x y)
	  ))))



(defun attack-location! (dungeon player x y)
  "attacks a given location.."

  (when-bind (monsters (cave-monsters dungeon x y))
    (let ((the-monster (car monsters)))
      ;; hack
      (play-sound 1)
      (attack-target! dungeon player the-monster x y nil)
      )))


(defun cmb-monster-attack! (dungeon player mon the-x the-y)
  "The monster attacks the player  at (the-x,the-y)."
  (dolist (the-attack (monster.attacks mon))
    (when (and (creature-alive? mon) (creature-alive? player))
      (attack-target! dungeon mon player the-x the-y the-attack))))

    
  
#||
  (let ((mon-name (monster.name mon)))
  
    (if (melee-hit-creature? mon player)
	(warn "'~a' hit the player.." mon-name)
	(warn "'~a' missed the player.." mon-name))
    ))
||#

#||
(defmethod melee-hit-creature? (attk target the-attack)
  (declare (ignore attk target the-attack))
  (error "not impl."))
||#
#||
(defmethod melee-inflict-damage! (attacker target the-attack)
  (declare (ignore the-attack))
  (error "Unknown combo ~s ~s" attacker target))
||#
  
