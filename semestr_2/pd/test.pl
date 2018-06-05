suma([],0).
suma([H|T],N):-suma(T,N1),N is N1+H.

odleglosc([H|T],H,O):-odleglosc(T,0,H,O).
odleglosc([H|T],E,O):-H\=E,odleglosc(T,E,O).
odleglosc([H|_],X,H,X).
odleglosc([H|T],X,E,Y):-H\=E,X1 is X+1,odleglosc(T,X1,E,Y).

lista_odl(_,[],[]):-!.
lista_odl(L,[H|T],O):-odleglosc(L,H,O1),lista_odl(L,T,O2),append([O1],O2,O).

elem_unik([],L,O):-!,length(L,O),!.
elem_unik([H|T],L,O):-member(H,L),elem_unik(T,L,O),!.
elem_unik([H|T],[H|T2],O):-elem_unik(T,T2,O),!.

focus(L,O):-elem_unik(L,A,B),lista_odl(L,A,C),suma(C,D),O is D/B.

addhead([],[]).
addhead([H|T],X):-focus(H,O),addhead(T,X1),append([[O|H]],X1,X).

przestaw([],[]):-!.
przestaw([X,Y|T],[Y,X|T]):-append([H1],_,X),append([H2],_,Y),H1>=H2,!.
przestaw([X|T],[X|T2]):-przestaw(T,T2),!.

babel(X,X):-przestaw(X,L),X=L.
babel(X,L):-przestaw(X,A),A\=X,babel(A,L).

focusort(X,O):-addhead(X,A),babel(A,B),delhead(B,O).

delhead([],[]).
delhead([H|T],X):-append([_],A,H),delhead(T,X1),append([A],X1,X).
