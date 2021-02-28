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
