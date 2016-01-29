(load "./lib/list.scm")

(define l 
  (cons 1 
    (cons 2 
      (cons 3 0))))

(define read-element
  (lambda (in)
    (if (<= in 2)
      (element-at l in)
      (- 0 1))))

(write (read-element (read)))

