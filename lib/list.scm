; List library

(load "./lib/core.scm")

; Pair encoding using the Cantor pair encoding

(define cons-acc
  (lambda (x y acc)
    (if (= x 0)
	(if (= y 0)
	    acc
	    (cons-acc (- y 1) 0 (+ acc 1)))
	(cons-acc (- x 1) (+ y 1) (+ acc 1)))))

(define car-acc
  (lambda (p acc-x acc-y acc)
    (if (= p acc)
	acc-x
	(if (= acc-y 0)
	    (car-acc p 0 (+ acc-x 1) (+ acc 1))
 	    (car-acc p (+ acc-x 1) (- acc-y 1) (+ acc 1))))))

(define cdr-acc
  (lambda (p acc-x acc-y acc)
    (if (= p acc)
	acc-y
	(if (= acc-y 0)
	    (cdr-acc p 0 (+ acc-x 1) (+ acc 1))
 	    (cdr-acc p (+ acc-x 1) (- acc-y 1) (+ acc 1))))))

(define cons
  (lambda (x y)
    (cons-acc x y 0)))

(define car
  (lambda (p)
    (car-acc p 0 0 0)))

(define cdr
  (lambda (p)
    (cdr-acc p 0 0 0)))

; Definition of lists based on pairs

(define nil 0)

(define list
  (lambda (hd tl)
    (cons hd tl)))

(define element-at
  (lambda (l n)
    (if (= n 0)
	(car l)
	(element-at (cdr l) (- n 1)))))

(define null?
  (lambda (lst)
    (= lst nil)))

(define length
  (lambda (lst)
    (if (null? lst)
	0
	(+ 1 (length (cdr lst))))))
