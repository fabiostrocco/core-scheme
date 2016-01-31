; Core scheme library

(define apply
 (lambda (f x)
   (f x)))

; Boolean operators

(define and 
  (lambda (x y)
    (if x
	y
	0)))

(define or
  (lambda (x y)
    (if x
	1
	y)))

(define not
  (lambda (x)
    (if x 0 x)))

; Numeric operators

(define <=
  (lambda (x y)
    (or (< x y)
	(= x y))))

(define >=
  (lambda (x y)
    (or (> x y)
	(= x y))))

(define <>
  (lambda (x y)
    (not (= x y))))

(define !
  (lambda (n)
    (if (= n 0)
	1
	(* (! (- n 1))
	   n))))

; Begin operators

(define begin
  (lambda (x y)
    y))

(define begin-three
  (lambda (x y z)
    (begin
      0 z)))
