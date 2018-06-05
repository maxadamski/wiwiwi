odl(H,L,X):-append(_,[H|T1],L),append(A,[H|_],T1),length(A,X),!.

uniq([],[]):-!.
uniq([H|T],X):-member(H,T),uniq(T,X),!.
uniq([H|T],[H|X]):-uniq(T,X),!.

sumodl(L,X):-uniq(L,A),sumodl(L,A,X),!.
sumodl(_,[],0):-!.
sumodl(L,[H|T],X):-sumodl(L,T,X1), odl(H,L,A), X is A+X1,!.

focus(L,X):-sumodl(L,A),uniq(L,B),length(B,C),X is A / C.

wstawsort(X,[],[X]):-!.
wstawsort(X,[H|T],[H|Y]):-focus(X,A),focus(H,B),A>B,wstawsort(X,T,Y).
wstawsort(X,[H|T],[X,H|T]):-focus(X,A),focus(H,B),A=<B.

focussort([],[]):-!.
focussort([H|T],X):-focussort(T,X1),wstawsort(H,X1,X),!.

odl1(X,[X|T],Y):-odl2(X,T,0,Y),!.
odl1(X,[_|T],Y):-odl1(X,T,Y),!.
odl2(X,[X|_],Y,Y):-!.
odl2(X,[H|T],A,Y):-X\=H,B is A+1,odl2(X,T,B,Y),!.
