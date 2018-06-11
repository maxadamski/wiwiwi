next(świnoujście,kołobrzeg,118).
next(świnoujście,szczecin,106).
next(kołobrzeg,ustka,117).
next(ustka,gdańsk,136).
next(gdańsk,toruń,170).
next(gdańsk,olsztyn,136).
next(olsztyn,toruń, 176).
next(olsztyn,białystok,238).
next(szczecin,bydgoszcz,257).
next(szczecin,gorzów-wlkp,111).
next(bydgoszcz,toruń,45).
next(bydgoszcz,poznań,142).
next(poznań,toruń,161).
next(gorzów-wlkp,poznań,170).
next(gorzów-wlkp,zielona-góra,119).
next(zielona-góra,poznań,155).
next(zielona-góra,wrocław,156).
next(toruń,warszawa,210).
next(toruń,łódź,179).
next(białystok,warszawa,199).
next(białystok,lublin,252).
next(poznań,łódź,202).
next(warszawa,radom,102).
next(warszawa,łódź,128).
next(radom,lublin,116).
next(radom,kielce,77).
next(wrocław,łódź,219).
next(łódź,radom,136).
next(łódź,częstochowa, 124).
next(częstochowa,kielce,132).
next(częstochowa,katowice,73).
next(katowice,kraków,80).
next(kielce,kraków, 115).
next(kielce,rzeszów,163).
next(lublin,kielce,176).
next(lublin,rzeszów,168).
next(kraków,rzeszów,176).

regular(rect(pt(X1, Y1), pt(X2, Y1), pt(X2, Y2), pt(X1, Y2))).

polaczenie(A,B) :- next(A, B, _).
polaczenie(A,B) :- next(A, C, _), polaczenie(C, B).

% P = A jest zbędny!
% zjazd(A, B, P) :- next(A, B, _), P = A.
zjazd(A, B, A) :- next(A, B, _).
zjazd(A, B, P) :- next(A, C, _), zjazd(C, B, P).

trasa(A, B, con(A, B)) :- next(A, B, _).
trasa(A, B, con(A, T)) :- next(A, C, _), trasa(C, B, T).

droga(A, B, [A,B]) :- next(A, B, _).
droga(A, B, [A|T]) :- next(A, C, _), droga(C, B, T).

convert([A,B], con(A, B)).
convert([A|B], con(A, U)) :- convert(B, U).

copy([], []).
copy([H|T], [H|Z]) :- copy(T, Z).

rev([],[]).
rev([H|T], [Z|H]) :- rev(T, Z).

odd([], []).
odd([A], [A]).
odd([H|[_|T]], [H|X]) :- odd(T, X).

even([], []).
even([_], []).
even([_|[H|T]], [H|X]) :- even(T, X).
