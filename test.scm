(load "./lib/list.scm")

(define factorial
  (lambda (n)
    (if (= n 0)
	1
	(* n (factorial (- n 1))))))

(begin
  (write (factorial (read)))
  0)
