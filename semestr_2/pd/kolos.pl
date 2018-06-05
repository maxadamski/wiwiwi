cyfry(0,[]):-!.
cyfry(A,L):-B is A // 10,cyfry(B,L1),H is A mod 10,append(L1,[H],L).

cyfryL([],[]).
cyfryL([H|T],L):-cyfryL(T,L1),cyfry(H,L2),append([L2],L1,L).

usun([],[]):-!.
usun([X,Y|T],T2):-Y=<X,usun([X|T],T2),!.
usun([X|T],[X|T2]):-usun(T,T2),!.

usunL([],[]).
usunL([H|T],X):-usunL(T,X1),usun(H,A),append([A],X1,X).

pol(X,Y):-length(X,A),pol(X,10^A,Y).
pol([],1,0).
pol([H|T],A,X):-B is A // 10,pol(T,B,X1),C is B*H,X is C+X1.

polL([],[]).
polL([H|T],X):-polL(T,X1),pol(H,A),append([A],X1,X).

przestaw([X],[X]):-!.
przestaw([X,Y|T],[Y,X|T]):-append([H1],_,X),append([H2],_,Y),H1>=H2,!.
przestaw([X|T],[X|T2]):-przestaw(T,T2).

babel(X,X):-przestaw(X,L),L=X.
babel(X,L):-przestaw(X,A),A\=X,babel(A,L).

addhead([],[]).
addhead([H|T],X):-cyfry(H,A),usun(A,B),pol(B,C),addhead(T,X1),append([[C,H]],X1,X).

cuthead([],[]):-!.
cuthead([H|T],X):-append([_],A,H),cuthead(T,X1),!,append(A,X1,X).

main(X,L):-addhead(X,A),babel(A,B),cuthead(B,L).
