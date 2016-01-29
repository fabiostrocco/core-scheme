(load "./lib/core.scm")

(define fib
  (lambda (n)
    (if (or
	 (= n 0)
	 (= n 1))
	1
	(+ (fib (- n 1)) (fib (- n 2))))))

(define n (read))

(write (fib n))
