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


con(A, B) :- next(A, B, _).
con(A, B) :- next(C, B, _), con(A, C).

%sum(X, Y, Sum) :- Sum is X + Y.

con(A, B, Dist) :- next(A, B, Dist).
con(A, B, Dist) :- next(C, B, DistNext), con(A, C, DistCon), Dist is DistNext + DistCon.

%pre(A, B, Pre) :- next(A, Pre, _), next(Pre, B, _).
pre(Pre, B, Pre) :- next(Pre, B, _).
pre(A, B, Pre) :- next(A, C, _), pre(C, B, Pre).

three([A, B, C]) :- A < B, B < C.

empty([]).

primes([2, 3, 5, 7, 11, 13]).

one([a]).

%sum(L, Sum) :- L = [], Sum = 0.
%sum([], 0).
%sum(L, Sum) :- L = [H|T], sum(T, TailSum), Sum is H + TailSum.
%sum([H|T], Sum) :- sum(T, TailSum), Sum is H + TailSum.

sum([], _) :- false.
sum([H], H).
sum([H|T], Sum) :- sum(T, TailSum), Sum is H + TailSum.

copy([], []).
copy([H|T], [CopyH|CopyT]) :- CopyH is H mod 2, copy(T, CopyT).

len([], 0).
len([_|T], Len) :- length(T, LenT), Len is LenT + 1.

append2([], L, L).
append2([H|L], R, [H|LR]) :- append2(L, R, LR).

rev([], []).
rev([H|T], Rev) :- rev(T, RevT), append(RevT, [H], Rev).

con_list(A, B, [A, B]) :- next(A, B, _).
con_list(A, B, [A|T]) :- next(A, Z, _), con_list(Z, B, T).
