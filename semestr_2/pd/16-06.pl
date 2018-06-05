dzielniki(X,L):-dzielniki(X,1,L).
dzielniki(X,X,[]):-!.
dzielniki(X,A,[A|Y]):-X mod A =:= 0, C is A+1, dzielniki(X,C,Y),!.
dzielniki(X,A,Y):-C is A+1, dzielniki(X,C,Y),!.

sumeven([],0):-!.
sumeven([H|T],X):-H mod 2 =:= 0, sumeven(T,X1), X is X1+H,!.
sumeven([_|T],X):-sumeven(T,X),!.

sumodd([],0):-!.
sumodd([H|T],X):-H mod 2 =\= 0, sumodd(T,X1), X is X1+H,!.
sumodd([_|T],X):-sumodd(T,X),!.

sum(X,L):-X mod 2 =:= 0, dzielniki(X,A), sumeven(A,L).
sum(X,L):-X mod 2 =\= 0, dzielniki(X,A), sumodd(A,L).

wstawsort(X,[],[X]):-!.
wstawsort(X,[Y|T1],[Y|T2]):-sum(X,A), sum(Y,B), A>B, wstawsort(X,T1,T2),!.
wstawsort(X,[Y|T],[X,Y|T]):-sum(X,A), sum(Y,B), A=<B,!.

main([],[]).
main([H|T],X):-main(T,X1), wstawsort(H,X1,X).
