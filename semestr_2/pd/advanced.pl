% # Advanced Prolog

% ## Using `call`

% Functional `map`
%
% Examples
%	?- map([0,1,2,3], plus(2), X).
%	X = [2,3,4,5]
%
map2([], _, []).
map2([H|T], Map, [MapH|MapT]) :-
	call(Map, H, MapH),
	map2(T, Map, MapT).

% Functional `filter`
%
% Examples:
%	even(N) :- N mod 2 =:= 0.
%
%	?- filter2([1,2,3,4,5,6], even, X).
%	X = [2, 4, 6]
%
filter2([], _, []).
filter2([H|T], Pred, Filter) :-
	filter2(T, Pred, FilterT),
	( call(Pred, H) ->
		Filter = [H|FilterT] ;
		Filter = FilterT ).

% Functional `reduce`
%
% Examples:
%	?- reduce2([1, 2, 3, 4], 0, plus, X).
%	X = 10
%
%	?- reduce2(["a", "b", "c"], "", concat, X).
%	X = "abc"
%
reduce2([], Base, _, Base).
reduce2(List, Base, Acc, Reduce) :-
	append(Init, [Last], List),
	reduce2(Init, Base, Acc, ReduceT),
	call(Acc, ReduceT, Last, Reduce).

% ## Using cuts


% member2(X, [X|_]). % ND
member2(X, [X|_]) :- !.
member2(X, [_|T]) :- member2(X, T).

snake(cobra).

likes(mary, X) :- snake(X), !, fail.
likes(mary, _).

different(X, X) :- !, fail.
different(_, _).

not(G) :- call(G), !, fail.
not(_).

same(X, Y) :- not(different(X, Y)).

max(X, Y, X) :- X >= Y.
max(X, Y, Y) :- Y >= X.

del([A|T], A, T).
del([H|T], A, [H|X]) :- del(T, A, X).


del_all([], _, []).
del_all([A|T], A, X) :- del_all(T, A, X), !.
del_all([H|T], A, [H|X]) :- del_all(T, A, X).

sublist(L, S) :- append(_, T, L), append(S, _, T).

% Merge sort

%merge([], [], []).
%merge([LH | LT], [RH | RT], ) :- append(LM, RM, X).

% Base converter


len([], 0).
len([_ | T], Len) :- len(T, LenT), Len is LenT + 1.

halves(I, L, R) :- append(L, R, I), length(L, LenL), length(R, LenR), 1 >= abs(LenL - LenR).

