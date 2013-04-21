;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: org.langband.engine -*-

#|

DESC: stores.lisp - code which deals with stores and their owners
Copyright (c) 2000-2003 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :org.langband.engine)

(defun is-store? (obj)
  (typep obj 'store))

;;; Current implementation ignores haggling, selling-season, buying-season, etc

(defmethod find-owner-for-house (level (house store)
				       &key
				       (var-obj *variant*)
				       (selection :random))
  
  (declare (ignore level))
  
  (let ((poss-owners (store.possible-owners house))
	(the-owner nil))
    
    (unless poss-owners
      (warn "Unable to find any possible owners for store ~a" house)
      (return-from find-owner-for-house nil))

    (ecase selection
      (:random
       (setf the-owner (rand-elm poss-owners))))
    
    (assert (not (eq the-owner nil)))

    (get-owner the-owner var-obj)
    ))


(defun add-owner-to-store-type! (owner store-type-id
				 &optional (var-obj *variant*))
  "The OWNER argument should be an owner, the STORE-TYPE-ID
should be an exisiting id."
  
  (let* ((v-obj var-obj)
	 (store-type (get-house store-type-id v-obj)))
    
    (if (and store-type (typep store-type 'store))
	(pushnew (owner.id owner) (store.possible-owners store-type))
	(warn "Unable to find store-type ~a" store-type-id))))
	 
(defun make-store-sales-list (variant store-id sale-list)
  "Returns a list of sale-priorities, given a sale-spec."
  (let ((ret-list '()))
    (dolist (i sale-list)
      (cond ((and (consp i) (eq (car i) 'obj))
	     (destructuring-bind (dummy &key id type (weight 1))
		 i
	       (declare (ignore dummy))
	       (cond (id
		      (let ((k (get-object-kind variant id)))
			(if k
			    (dotimes (j weight)
			      (push k ret-list))
			    (warn "Unable to find kind for obj-id ~s" id))))
		     (type
		      (warn "Type-spec for store-objs not implemented."))
		     (t
		      (warn "Neither type or id is mentioned for obj-spec ~s for store ~s" i store-id)))
	       ))
	    (t
	     (warn "Unknown format for spec ~s for store ~s" i store-id))))
    
    ret-list))


(defun define-store (id &key (type 'store) name number
		     (sells nil) (buys nil) (owner :random) (no-items nil))
  "creates a store object and adds it to the appropriate table"
;;  (declare (ignore args))
  (let ((var-obj *variant*)
	(store (make-instance type :id id :name name :number number :owner owner)))
    
    (when (and (eq type 'store) sells)
      (setf (store.sells store) sells))
    
    (when (and (eq type 'store) buys)
      (setf (store.buys store) buys))
    
    ;; hackish
    (unless no-items
      (setf (house.items store) (make-container (store.item-limit store) 'items-in-store)))

    (establish-house& var-obj store)

    (when (and number (numberp number))
      ;; add to numbered position
      (establish-house& var-obj store :house-key number))
    
    store))

(defun define-store-owner (&key store-type id name purse max-greed
			   min-greed haggle-num tolerance race
			   picture special-owner)
  "creates an owner and adds him or her to appropriate tables"

;;  (warn "Looking for ~s in ~s" race (get-races-as-a-list variant))
  (let* ((race-obj (if (and race (or (symbolp race)
				     (stringp race)))
		       (get-char-race race)
		       race))
	 (owner (make-instance 'store-owner :id id :name name
			      :purse purse :max-greed max-greed
			      :min-greed min-greed :haggle-num haggle-num
			      :picture picture
			      :tolerance tolerance :race race-obj))
	 (var-obj *variant*))

    (when (and picture (not (image-exists? picture)))
      (warn "Unable to find picture ~s for store-owner ~s." picture id)
      (setf (owner.picture owner) nil))

    ;; we add it to the owner-table
    (establish-owner& var-obj owner)
    
    ;; we just want generic owners to the relation table
    (unless special-owner
      (add-owner-to-store-type! owner store-type var-obj))

    owner))

(defmethod get-price ((object active-object) (store store))
  (let* ((okind (aobj.kind object))
	 (default-price (object.cost okind)))

    ;; skip storekeeper, charisma, ...
    (if (and (numberp default-price) (>= default-price 0))
	(floor (* 1.5 default-price) 1)
	0)))


(defmethod get-offer ((object active-object) (store store))
  (int-/ (get-price object store) 2)) ;; decent value, eh?

(defun %print-shop-message! (str)
  (print-message! str +term-yellow+))
;;  (let ((line (- (get-last-console-line +dialogue-frame+) 5)))
;;    (put-coloured-line! +term-yellow+ str 2 line)))



(defun %store-select-item (low top)
  (let ((the-char (read-one-character)))
    (cond ((eql the-char #\Escape)
	   nil)
	  ((characterp the-char)
	   (let ((char-num (a2i the-char)))
	     (cond ((and (<= low char-num)
			 (>= top char-num))
		    char-num)
		   (t
		    (put-coloured-line! +term-white+ "Illegal selection!" 0 0)
		    nil))))
	  (t
	   #-cmu
	   (put-coloured-line! +term-white+ "Odd return-value!" 0 0)
	   nil))))
		 

(defun %store-buy-item (player level store)
  (declare (ignore level))
  (block buying
    (let* ((var-obj *variant*)
	   (items (store.items store))
	   (item-len (items.cur-size items)))
  
      (put-coloured-str! +term-yellow+
			 (format nil "(Items ~a-~a, ESC to exit) Which item are you interested in?"
				 (i2a 0) (i2a (1- item-len)))
			 0 0)
      (let ((selected (%store-select-item 0 (1- item-len))))
	(when (and selected (numberp selected))
	  (let* ((retval nil)
		 (act-obj (item-table-find items selected))
		 (the-price (get-price act-obj store))
		 (backpack (aobj.contains (player.inventory player))))
	    ;;(warn "Buying ~s for ~s" act-obj the-price)
	    (unless (<= the-price (player.gold player))
	      (%print-shop-message! "You cannot afford that item!")
	      (return-from buying nil))

	    (unless (item-table-more-room? backpack)
	      (%print-shop-message! "No room in backpack!")
	      (return-from buying nil))

	    (cond ((= 1 (aobj.number act-obj))
		   (possible-identify! player act-obj) ;; fix?
		   (item-table-add! backpack act-obj)
		   (item-table-remove! items act-obj)
		   (setf retval act-obj))
		  
		  ((> (aobj.number act-obj) 1)
		   (let ((new-obj (copy-active-object var-obj act-obj)))
		     (decf (aobj.number act-obj))
		     (setf (aobj.number new-obj) 1)
		     (item-table-add! backpack new-obj)
		     (possible-identify! player new-obj)
		     (setf retval new-obj))
		   ))

	    ;; add identify for it
	    (decf (player.gold player) the-price)
	    (bit-flag-add! *redraw* +print-gold+)
	    
	    
	    retval)
	  )))))

(defun %store-sell-item (player level store)
  
  (let ((dungeon (level.dungeon level)))
    (block selling

      (when-bind (selection (select-item dungeon player '(:backpack :equip)
					 :prompt "Sell which item? "
					 :where :backpack))
      

	(let* ((the-table (get-item-table dungeon player (car selection)))
	       (removed-obj (item-table-remove! the-table (cdr selection) :only-single-items t)))

	  (when removed-obj

	    ;; does the shop want to buy that kind of object?
	    (let ((might-buy (store-buys-item? removed-obj store)))
	      (unless might-buy
		(%print-shop-message! "- I don't buy such items.")
		;; put it back.
		(item-table-add! the-table removed-obj)
		(return-from selling nil)))

	    ;; does the shop have any room?
	    (let ((shop-items (store.items store)))
	      (unless (item-table-more-room? shop-items)
		(%print-shop-message! "- I have no more room in the store.")
		;; put it back.
		(item-table-add! the-table removed-obj)
		(return-from selling nil)))
	    
	    ;; can we get a decent price?
	    (let ((price (get-offer removed-obj store)))
	      (cond ((plusp price)
		     (%print-shop-message! "- It's a deal.")
		     ;; add to shop
		     (item-table-add! (store.items store) removed-obj)
		     (incf (player.gold player) price)
		     (return-from selling t))
		    ;; no decent price
		    (t
		     (%print-shop-message! "- That item is worthless, I don't want it.")
		     (item-table-add! the-table removed-obj)
		     (return-from selling nil))))

	    nil))
	
	))))


(defmethod display-house ((player player) (store store) &key (offset 0))

  (declare (ignore offset))
  
  (let ((store-name (store.name store))
	(store-limit 50000)
	(the-owner (house.owner store))
	(owner-name "Bob")
	(owner-race "Human"))

    (when (and the-owner (typep the-owner 'store-owner))
      (setf owner-name (owner.name the-owner))
      (let ((the-race (owner.race the-owner)))
	
	(when (and the-race (symbolp the-race))
	  (setf the-race (get-char-race the-race)))
	
	(when (and the-race (typep the-race 'character-race))
	  (setf owner-race (race.name the-race))))
      
      (let ((poss-limit (owner.purse the-owner)))
	(when (and poss-limit (plusp poss-limit))
	  (setf store-limit poss-limit))))

    (clear-window *cur-win*) ;; hack

    ;; big empty space when no graphics
    (when (use-images?)
      ;; hackish, improve later

      (let ((owner-picture (owner.picture the-owner)))
	(when (and owner-picture (stringp owner-picture))

	  (paint-gfx-image& owner-picture 1 1)
	  
;;      (load-scaled-image& "./graphics/people/grim-elf.bmp" -1 6 5)
;;      (paint-image& "./graphics/people/grim-elf.bmp" 1 1))

	  )))
	  
    (let ((left-col 20)
          (desc-line 7)
	  (last-line (get-last-console-line)))

      
      (put-coloured-str! +term-yellow+ (format nil "~a" store-name) left-col 1)
      (put-coloured-str! +term-white+ (format nil "Owned by: ~a (~a)" owner-name owner-race) left-col 2)
      (put-coloured-str! +term-white+ (format nil "Max purchase value: ~a AU" store-limit) left-col 3)

      (put-coloured-str! +term-white+ "Item Description" 3 desc-line)
      (put-coloured-str! +term-white+ "Weight" 60 desc-line)
      (put-coloured-str! +term-white+ "Price" 72 desc-line)

      (put-coloured-str! +term-white+ "Gold Remaining:" 53 (- last-line 3))
      
      (put-coloured-str! +term-white+
			 (format nil "~9d" (player.gold player))
			 68 (- last-line 3))

      ;; pass last-line as info here
      (item-table-print (house.items store) :store store :start-y (1+ desc-line))

      (put-coloured-str! +term-yellow+ "ESC" 1 (1- last-line))
      (put-coloured-str! +term-white+ ") Exit from building." 4 (1- last-line))
      (put-coloured-str! +term-yellow+ "g" 31 (1- last-line))
      (put-coloured-str! +term-white+ ") Get/purchase item." 32  (1- last-line))
      (put-coloured-str! +term-yellow+ "d" 31 last-line)
      (put-coloured-str! +term-white+ ") Drop/sell item." 32 last-line)

      (put-coloured-str! +term-white+ "You may: " 0 (- last-line 2)))

   
    t))

(defun %shop-input-loop (player level store)
  
  (block input-loop
    (loop
     ;; this should vary depending on size and stuff
     (set-cursor-to *cur-win* :input 10 21)
   
     (let ((val (read-one-character)))
       (flush-messages! t) ;; forced
       ;;(warn "shop-loop got ~s" val) 
       (cond ((or (eql val #\g)
		  (eql val #\p))
	      (when-bind (retval (%store-buy-item player level store))
		(display-house player store)
		(update-inventory-row player)
		(put-coloured-line! +term-white+ "" 0 0)))
     
	     ((or (eql val #\d)
		  (eql val #\s))
	      (%store-sell-item player level store)
	      (display-house player store)
	      (update-inventory-row player)
	      (put-coloured-line! +term-white+ "" 0 0))

	    
	     ((or (eql val #\Escape)
		  (eql val #\Q))
	      (return-from input-loop t))
	    
	     (t
	      (warn "Unknown key read: ~s" val)))
     
       ;;     (put-coloured-line! +term-white+ "" 0 0)
       ))))


(defmethod visit-house (level (house store))
  "Visits the given store."

  (unless (activated? house)
    (activate-object house))

  (flush-messages! t)
  (with-dialogue ()
    (clear-window *cur-win*)
    (display-house *player* house :offset 0)
    
    (%shop-input-loop *player* level house)
    ))



;; hackish  create/delete/maint
(defmethod store-generate-object ((variant variant) (the-store store))
  "this is just for a regular store, not a black market"

  (when-bind (sells (store.sells the-store))
    (when-bind (kind (rand-elm sells))
      (when (typep kind 'object-kind)
	(let ((aobj (create-aobj-from-kind kind :variant variant)))
	  ;; possibly add magic
	  (apply-magic! variant aobj (store.object-depth the-store) :allow-artifact nil)
	  (store-mass-produce! variant the-store aobj)
	  (return-from store-generate-object aobj)))))

  (warn "Fell through in obj-generation for store ~s" the-store)
    
  nil)


(defun store-delete-obj! (the-store &optional obj-key)
  "just wipes an object.."
  (let* ((store-items (store.items the-store))
	 (cur-size (items.cur-size store-items))) 
    (when (plusp cur-size)
      (let ((key (if obj-key obj-key (random cur-size))))
	;;(warn "Removing ~s" key)
	(item-table-remove! store-items key)))))

(defun store-add-obj! (variant the-store)
  (when-bind (new-obj (store-generate-object variant the-store))
    ;;(warn "Adding ~s" (object.name new-obj))
    (item-table-add! (store.items the-store) new-obj)))

(defun store-empty? (variant store)
  "Is the store devoid of objects?"
  (declare (ignore variant))
  
  (let* ((items (store.items store))
	 (objs (items.objs items))
	 (count 0))
    (loop for x across objs
	  when x
	  do (incf count))
    
    (zerop count)))
    
(defun equip-store! (variant store)
  ;;(warn "EQUIP ~a" (store.id store))
  (dotimes (j 10)
    (store-maintenance! variant store)))

;;(trace equip-store!)

(defmethod activate-object ((obj store) &key)

  (let ((res-obj (call-next-method)))
    (unless (eq res-obj obj)
      (error "Something fu with store-obj ~a" res-obj)))
  
  (when-bind (sells (store.sells obj))
    ;; late-init basically
    (setf (store.sells obj) (make-store-sales-list *variant* (store.id obj) sells)))
    
  ;;(equip-store! *variant* obj)
  
  obj)


(defmethod store-maintenance! ((variant variant) (the-store store))
  "hackish, fix later."

  (let ((min (store.min-items the-store))
	(max (store.max-items the-store))
	(limit (items.max-size (store.items the-store)))
	(turnover (store.turnover the-store))
	(cur-num (items.cur-size (store.items the-store)))
	)

    ;; we have some turnover
    (decf cur-num (randint turnover))

    (when (> cur-num max) (setf cur-num max))
    (when (< cur-num min) (setf cur-num min))
    (when (minusp cur-num) (setf cur-num 0))

    (while (> (items.cur-size (store.items the-store)) cur-num)
      ;;(warn "-Compare ~s ~s" (items.cur-size (store.items the-store)) cur-num)
      (store-delete-obj! the-store))

    ;; now, let us get some new stuff in

    (setf cur-num (items.cur-size (store.items the-store)))

    (incf cur-num (randint turnover))
    
    (when (> cur-num max) (setf cur-num max))
    (when (< cur-num min) (setf cur-num min))
    (when (>= cur-num limit) (setf cur-num (1- limit)))

    (while (< (items.cur-size (store.items the-store)) cur-num)
      ;;(warn "+Compare ~s ~s" (items.cur-size (store.items the-store)) cur-num)
      (store-add-obj! variant the-store))

    ;;(warn "Return")
    the-store))
 
