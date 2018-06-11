
sum_([], 0).
sum_([Head|Tail], Sum) :-
	sum_(Tail, TailSum),
	Sum is Head + TailSum.

product_([], 1).
product_([Head|Tail], Product) :-
	product_(Tail, TailProduct),
	Product is Head * TailProduct.

fib1(0, 0) :- !.
fib1(1, 1) :- !.
fib1(N, Fib) :-
	A is N - 1,
	B is N - 2,
	fib1(A, FibA),
	fib1(B, FibB),
	Fib is FibA + FibB.
