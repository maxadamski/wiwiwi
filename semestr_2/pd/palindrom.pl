
% ostatni element listy

last_([_], _).
last_([_|T], X) :- last_(T, X).

% lista bez ostatniego elementu

without_last_([_],[]).
without_last_([H|WithoutLastT], [H|T]) :- without_last_(WithoutLastT, T).

% długość listy

len_([], 0).
len_([_|T], Length) :- len_(T, LengthT), Length is LengthT + 1.

% konkatenacja dwóch list

append_([], List, List).
append_([H|T], Right, [H|AppendT]) :- append_(T, Right, AppendT).

% odwracanie listy

reverse_([], []).
reverse_([H|T], Reverse) :- append(ReverseT, [H], Reverse), reverse_(T, ReverseT).

%
% zajęcia 5
%

% sprawdzenie palindromu

plaindrome([_]).
plaindrome([A, A]).
plaindrome([Last|Tail]) :- append(TailWithoutLast, [Last], Tail), plaindrome(TailWithoutLast).

% rozdziel listę na dwie połówki

% halves([LeftH|T], [LeftH|], RightH|]) :- split halves
% append([H|T], [Y], Lwej)
% H = Y

% suma tablicy

sum_([], 0).
sum_([Head|Tail], X) :- sum_(Tail, TailSum), X is Head + TailSum.

% lista parzystych i nieparzystych

even_odd([], [], []).
even_odd([H|T], [H|ET], OT) :- even_odd(T, ET, OT), H mod 2 =:= 0.
even_odd([H|T], ET, [H|OT]) :- even_odd(T, ET, OT), H mod 2 =\= 0.

% n-głowa + n-ogon

nhead(0, _, []).
nhead(N, [H|T], [H|R]) :- M is N - 1, nhead(M, T, R).

ntail(N, X, X) :- length(X, Len), N >= Len.
ntail(N, [_|T], Tail) :- ntail(N, T, Tail).

% podział listy na podlisty o długościach n_i

splits([], [], []).

splits([SplitH|SplitT], List, [HeadSplit|Split]) :-
	nhead(SplitH, List, HeadSplit),
	append(HeadSplit, RemainingList, List),
	splits(SplitT, RemainingList, Split).

