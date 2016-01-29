(load "./lib/core.scm")

(define counter 0)

(define f
  (lambda (g) (g)))

(define g
  (lambda ()
    (if (>= counter 1000)
	0
	(begin
	  (set! counter (+ counter 1))
	  (f g)))))

(begin
  (f g)
  (write counter))

