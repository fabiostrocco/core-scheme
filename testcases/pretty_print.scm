(define x 0)
(define y (lambda (x y) (x y)))
(define z (set! x 1))
(if x (y (lambda (x) x) x) 0)
