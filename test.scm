; Import list library
(load "./lib/list.scm")

; Factorial function definition
(define factorial
  (lambda (n)
    (if (= n 0)
	1
	(* n (factorial (- n 1))))))

; Main function. It reads a number n from the input, 
; and prints the factorial of n.
(begin
  (write (factorial (read)))
  0)
