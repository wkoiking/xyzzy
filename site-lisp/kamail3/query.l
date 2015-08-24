;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: query.l 740 2006-10-22 17:32:24Z torihat $
;
; kamail3/query.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/query")

(in-package "kamail3")

(defvar *query-last-history* nil)
(defvar *query-search-history* nil)
(defvar *query-search-history-max* 50)

(defvar *query-last-query* nil)
(defvar *query-last-result* nil)
(defvar *query-last-folder* nil)

(defvar *query-result-max* 200)
(defvar *query-result-offset* 0)

(defvar *query-history-file*
  (merge-pathnames "query-history.dat" *data-directory*))

(defstruct query
  string
  query
  attrs
  order
  folder
  tag)

(defun query-history-load ()
  (when (file-exist-p *query-history-file*)
    (setq *query-search-history*
	  (junk::data-read-from-file *query-history-file*))))

(defun query-history-save ()
  (junk::data-write-to-file *query-history-file*
			    *query-search-history*))

#|
(defun query-equal (a b)
  (or (and (query-string a)
	   (equal (query-string a)
		  (query-string b)))
      (and (equal (query-query a)
		  (query-query b))
	   (equal (query-attrs a)
		  (query-attrs b))
	   (equal (query-order a)
		  (query-order b))
	   (equal (query-folder a)
		  (query-folder b))
	   (equal (query-tag a)
		  (query-tag b)))))
|#

(defun query-equal (a b)
  (and (equal (query-string a)
	      (query-string b))
       (equal (query-query a)
	      (query-query b))
       (equal (query-attrs a)
	      (query-attrs b))
       (equal (query-order a)
	      (query-order b))
       (equal (query-folder a)
	      (query-folder b))
       (equal (query-tag a)
	      (query-tag b))))

(defun query-history-add (query)
  (setq *query-last-history* query)
  (push query *query-search-history*)
  (setq *query-search-history*
	(delete-duplicates *query-search-history*
			   :test #'query-equal
			   :from-end t))
  (when (< *query-search-history-max* (length *query-search-history*))
    (setq *query-search-history*
	  (nbutlast *query-search-history*
		    (- (length *query-search-history*) *query-search-history-max*))))
  (query-history-save))

(defun query-string-or-null (string)
  (if string
      (if (string-match "^[ \n\t]*$" string)
	  nil
	string)))

(defun query-input-folder (default)
  (let ((folder (completing-read "Folder: "
				 (mapcar #'folder-name *folder-list-special*)
				 :default default)))
    (query-string-or-null folder)))

(defun query-input-tag (default)
  (let ((tag (completing-read "Tag: "
			      *attr-tags-list*
			      :default default)))
    (query-string-or-null tag)))

(defvar *query-search-history-minibuf* nil)

(defun query-input-string (default)
  (let (history query)
    (dolist (q *query-search-history*)
      (let ((qstring (query-string q)))
	(when qstring
	  (pushnew qstring *query-search-history-minibuf* :test #'equal))))
    (setq query (read-string "Query: "
			     :default default
			     :history *query-search-history-minibuf*))
    (query-string-or-null query)))

(defun query-input-query (default)
  (let ((query (read-string "Query: "
			    :default default)))
    (query-string-or-null query)))

(defun query-input-attrs (default)
  (let ((attrs (read-string "Attribute: "
			    :default default)))
    (query-string-or-null attrs)))

(defun query-attr-split (attr)
  (let (attrs)
    (when (setq attr (query-string-or-null attr))
      (dolist (at (split-string attr "\n"))
	(push at attrs)))
    (nreverse attrs)))

(defun query-input-order (default)
  (let ((order (read-string "Order: "
			    :default default)))
    (query-string-or-null order)))

(defun query-parse-date-range (num type)
  (let* ((days (* (if (< 0 num)
		      num
		    (- 0 num))
		  (cond ((equal "d" type)
			 1)
			((equal "w" type)
			 7)
			((equal "m" type)
			 30) ; –Ê“|‚È‚Ì‚ÅŒÅ’è‚Å
			((equal "y" type)
			 365))))
	 (start-time (- (get-universal-time)
			(* days 24 60 60))))
    (format nil "date ~A ~A"
	    (if (< 0 num) "NUMGE" "NUMLT")
	    (format-date-string *query-date-format* start-time))))

(defvar *query-date-format* "%Y-%m-%d")

(defvar *query-date-operator-list*
  '(("<"  . "NUMLT")
    ("<=" . "NUMLE")
    ("=<" . "NUMLE")
    (">"  . "NUMGT")
    (">=" . "NUMGE")
    ("=>" . "NUMGE")))

(defun query-date-operator (type)
  (cdr (assoc type *query-date-operator-list* :test #'equal)))

(defun query-parse-date-condition (type datestr)
  (let ((operator (query-date-operator type)))
    (when operator
      (let ((y (parse-integer (substring datestr 0 4)))
	    (m (parse-integer (substring datestr 4 6)))
	    (d (parse-integer (substring datestr 6 8))))
	(format nil "date ~A ~A"
		operator
		(format-date-string *query-date-format*
				    (encode-universal-time 0 0 0 d m y)))))))

(defvar *query-attr-number-list*
  '("size" "date" "score"))

(defun query-attr-number-p (attr)
  (member-if #'(lambda (x)
		 (string-match x attr))
	     *query-attr-number-list*))

(defun query-string-parse (query-string)
  (let (rest
	parts
	query-list
	attr-list
	order)
    (dolist (part (split-string query-string " "))
      (cond ((string-match "order\\([<>]\\)\\([^ ]+\\)" part)
	     (let* ((ope (match-string 1))
		    (at (match-string 2))
		    (num-p (query-attr-number-p at)))
	       (setq order (format nil "~A ~A"
				   at
				   (if (equal ope "<")
				       (if num-p "NUMA" "STRA")
				     (if num-p "NUMD" "STRD"))))))
	    ((string-match "date:\\(-?[0-9]+\\)\\([dwmy]\\)" part)
	     (let ((cond (query-parse-date-range (parse-integer (match-string 1))
						 (match-string 2))))
	       (when cond
		 (push cond attr-list))))
	    ((string-match "date\\([<>=]+\\)\\([0-9]\\{8\\}\\)" part)
	     (let ((cond (query-parse-date-condition (match-string 1)
						     (match-string 2))))
	       (when cond
		 (push cond attr-list))))
	    ((string-match "\\([-0-9a-z,@]+\\)~\\([^ ]+\\)" part)
	     (push (format nil "~A STRRX ~A"
			   (match-string 1)
			   (match-string 2)) attr-list))
	    ((string-match "\\([-0-9a-z,@]+\\)=\\([^ ]+\\)" part)
	     (let* ((ope (match-string 1))
		    (at (match-string 2))
		    (num-p (query-attr-number-p ope)))
	       (push (format nil "~A ~:[STREQ~;NUMEQ~] ~A"
			     ope
			     num-p
			     at)
		     attr-list)))
	    ((string-match "\\([-0-9a-z,@]+\\):\\([^ ]+\\)" part)
	     (push (format nil "~A STRINC ~A"
			   (match-string 1)
			   (match-string 2)) attr-list))
	    (t
	     (push part query-list))))
    (when (or query-list attr-list)
      (when query-list
	(let (temp
	      saved)
	  (dolist (query (reverse query-list))
	    (if (equal query "[RX]")
		(setq saved query)
	      (if saved
		  (progn
		    (push (concat saved " " query) temp)
		    (setq saved nil))
		(push query temp))))
	  (setq query-list temp)))
      (make-query :string query-string
		  :query (junk::string-join (reverse query-list) " AND ")
		  :attrs (junk::string-join (reverse attr-list) "\n")
		  :order (or order *folder-order-default*)))
    ))

(defun query-parse (q)
  (let ((qstring (query-string q)))
    (when (stringp qstring)
      (setq q (query-string-parse qstring)))
    (let ((query (query-query q))
	  (attrs (query-attr-split (query-attrs q)))
	  (order (query-order q))
	  (folder (query-folder q))
	  (tag (query-tag q))
	  f
	  result)
      (when (and folder
		 (setq f (folder-get folder)))
	(when (folder-attrs f)
	  (setq attrs (append attrs (folder-attrs f)))))
      (when tag
	(setq attrs (append attrs
			    (list (attr-tag-query tag)))))
      (values query attrs order))))

(defun query-search ()
  (interactive)
  (let* ((qstring (query-input-string (and (query-p *query-last-history*)
					   (query-string *query-last-history*))))
	 (query (make-query :string qstring)))
    (when query
      (when *debug-mode*
	(msgbox "~S" query))
      (query-execute query))))

(defun query-search-detail ()
  (interactive)
  (let ((query (query-input-query (and (query-p *query-last-history*)
				       (query-query *query-last-history*))))
	(attrs (query-input-attrs (and (query-p *query-last-history*)
				       (query-attrs *query-last-history*))))
	(order (query-input-order (and (query-p *query-last-history*)
				       (query-order *query-last-history*))))
	folder
;	(folder (query-input-folder (and (query-p *query-last-history*)
;					 (query-folder *query-last-history*))))
	tag
;	(tag (query-input-tag (and (query-p *query-last-history*)
;				   (query-tag *query-last-history*))))
	)
    (query-execute (make-query :query  query
			       :attrs  attrs
			       :order  order
			       :folder folder
			       :tag    tag))
    ))

(defun query-search-and-delete (q)
  (let ((*query-result-offset* 0)
	(*query-result-max* -1)
	(cnt 0)
	result
	dirs
	cmd-list)
    (declare (special *query-result-offset*
		      *query-result-max*))
    (unless (setq result (query-search-do q))
      (return-from query-search-and-delete))
    (he-purge-files result)))

(defun query-search-do (q &optional (new-p t))
  (unless (query-p q)
    (return-from query-search-do))
  (multiple-value-bind (query attrs order)
      (query-parse q)
    (when *debug-mode*
      (msgbox "query: ~S~%attrs: ~S~%order: ~S" query attrs order))
    (unless (or query attrs)
      (return-from query-search-do))
    (when new-p
      (setq *query-result-offset* 0))
    (long-operation
      (he-search-do query
		    attrs
		    order
		    *query-result-max*
		    *query-result-offset*))))

(defun query-execute (q &optional folder-name (new-p t))
  (unless (query-p q)
    (return-from query-execute))
  (let ((result (query-search-do q new-p)))
    (unless (heresult-p result)
      (return-from query-execute))
    (unless folder-name
      (query-history-add q))
    (summary-window-set)
    (summary-mode)
    (setq *summary-message-list* (heresult-documents result))
    (setq *query-last-query* q)
    (setq *query-last-result* result)
    (setq *query-last-folder* folder-name)
    (summary-print)
    (query-summary-mode-line q result folder-name)
    (goto-char (point-min))
    (when *debug-mode*
      (msgbox "query: ~S~%~%history: ~S~%~%folder: ~S"
	      *query-last-query*
	      *query-last-history*
	      *query-last-folder*))
    ))

(defun query-search-next ()
  (interactive)
  (unless (heresult-p *query-last-result*)
    (kamail3-message "No history.")
    (return-from query-search-next))
  (unless (and (setq number (heresult-number *query-last-result*))
	       (< (+ *query-result-max*
		     *query-result-offset*)
		  number))
    (kamail3-message "No next page.")
    (return-from query-search-next))
  (incf *query-result-offset* *query-result-max*)
  (query-execute *query-last-query* *query-last-folder* nil))

(defun query-search-prev ()
  (interactive)
  (unless (heresult-p *query-last-result*)
    (kamail3-message "No history.")
    (return-from query-search-prev))
  (unless (and (setq number (heresult-number *query-last-result*))
	       (< 0 *query-result-offset*))
    (kamail3-message "No previous page.")
    (return-from query-search-prev))
  (setq *query-result-offset* (max (- *query-result-offset*
				      *query-result-max*)
				   0))
  (query-execute *query-last-query* *query-last-folder* nil))

(defun query-summary-mode-line (q result &optional folder-name)
  (let ((number (or (heresult-number result) 0))
	(string (query-string q))
	(query (query-query q))
	(attr  (query-attrs q))
	(folder (query-folder q)))
    (setq mode-line-format (format nil "%b: ~@[~A~]~@[ \"~A\"~]~@[ \"~A\"~] (~D-~D/~D)"
				   folder-name
				   (if folder-name
				       nil
				     (if string
					 string
				       query))
				   (if folder-name
				       nil
				     (if string
					 nil
				       (if attr
					   (substitute-string attr "\n" " ")
					 nil)))
				   (+ *query-result-offset* 1)
				   (min (+ *query-result-offset*
					   *query-result-max*)
					number)
				   number))
    ))

(defun query-folder-name-default (q)
  (let ((qstring (query-string q)))
    (if qstring
	qstring
      (let ((query (query-query q))
	    (attrs (query-attrs q))
	    items)
	(dolist (s (append (list query)
			   (query-attr-split attrs)))
	  (let ((item (query-string-or-null s)))
	    (when item
	      (push item items))))
	(when items
	  (junk::string-join (reverse items)))))))
