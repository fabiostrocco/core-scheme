(load "./lib/core.scm")

(define counter 0)

(define plus-one
  (lambda (tries)
    (if (> tries 0)
	(begin
	  (set! counter (+ counter 1))
	  (plus-one (- tries 1))) 
        0)))

(begin
  (plus-one 80)
  (write counter))
