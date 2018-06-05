
% pomocnicze operacje list

max_in([A], A).
max_in([H | T], X) :- max_in(T, R), X is max(H, R).

min_in([A], A).
min_in([H | T], X) :- min_in(T, R), X is min(H, R).

len([], 0).
len([_ | T], X) :- len(T, Len), X is 1 + Len.

sum([], 0).
sum([H | T], X) :- sum(T, Sum), X is H + Sum.

avg(L, X) :- sum(L, Sum), len(L, Len), X is Sum / Len.

find(E, [E | _], I, I).
find(E, [H | T], I, Find) :- E \= H, J is I + 1, find(E, T, J, Find).
find(E, L, I) :- find(E, L, 1, I).

unique([], []).
unique([H | T], Unique) :- unique(T, Unique), member(H, Unique), !.
unique([H | T], [H | Unique]) :- unique(T, Unique).

setdiff([], _, []).
setdiff([H | T], L, Diff) :- member(H, L), !, setdiff(T, L, Diff).
setdiff([H | T], L, [H | Diff]) :- setdiff(T, L, Diff).

% A/B: poczatek/koniec zakresu [A,B]
% L: lista wejsciowa
% I: sprawdzany indeks
slice(I, _, B, _, []) :- I > B, !.
slice(I, A, B, [_ | T], Slice) :- I < A, !, J is I + 1, slice(J, A, B, T, Slice).
slice(I, A, B, [H | T], [H | Slice]) :- J is I + 1, slice(J, A, B, T, Slice).
slice(A, B, L, Slice) :- slice(1, A, B, L, Slice).

% specyfczne zadania

% liczby kongruentne

congruent2(_, _, []).
congruent2(K, U, [H | T]) :- U is H mod K, congruent2(K, U, T).

congruent(I, L, I) :- congruent2(I, _, L), !.
congruent(I, L, N) :- I > 1, J is I - 1, congruent(J, L, N).
congruent(L, N) :- max_in(L, Max), congruent(Max, L, N).

% odleglosc w alfabecie

letter(Letter, Code) :-
	find(Letter, [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z], Code).
letter_distance(A, B, N) :- letter(A, U), letter(B, V), N is abs(U - V).

very_unique(L, [], L).
very_unique(L, [Set | Sets], Unique) :- setdiff(L, Set, Z), very_unique(Z, Sets, Unique).


% obsluga kluczy (jak ktos lubi)

coder(K, K).

encode([], []).
encode([V | T], [K/V | Encoded]) :- coder(V, K), encode(T, Encoded).

decode([], []).
decode([_/V | T], [V | Decoded]) :- decode(T, Decoded).

kinsert(A, [], [A]).
kinsert(AK/AV, [BK/BV | T], [AK/AV, BK/BV | T]) :- AK =< BK.
kinsert(AK/AV, [BK/BV | T], [BK/BV | Insert]) :- AK > BK, kinsert(AK/AV, T, Insert).

% insertion sort

key(_, K, K).

insert(_, A, [], [A]).
insert(L, A, [B | T], [A, B | T]) :-
	key(L, A, X), key(L, B, Y), X =< Y.
insert(L, A, [B | T], [B | Insert]) :-
	key(L, A, X), key(L, B, Y), X > Y, insert(L, A, T, Insert).

isort(_, [], []).
isort(L, [H | T], Sorted) :- isort(L, T, Z), insert(L, H, Z, Sorted).
isort(L, Sorted) :- isort(L, L, Sorted).

% rozwiazanie


