
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

% liczby pierwsze

divisors(N, [Test|Tail]) :-
	N > 0,
	divisors(N, Tail),
	append(_, [Last], Tail),
	Test is Last + 1,
	N mod Test =:= 0.

divisors(N, Tail) :-
	N > 0,
	divisors(N, Tail),
	append(_, [Last], Tail),
	Test is Last + 1,
	N mod Test =\= 0.

divisors(N, [1]) :- N > 0.

prime(N) :- divisors(N, [N, 1]).

%anyfactor(X, 2) :- factor(X, Root).
%anyfactor(X, Root) :- Dec is Root - 1, anyfactor(X, Root), anyfactor(X, Dec).

%prime(X) :- sqrt(X, Root), anyfactor(X, Root).


% selection sort

min(A, B, B) :- A >= B.
min(A, B, A) :- A < B.

min([X], X).
min([A|T], X) :- min(A, B, X), min(T, B).

%ssort([], []).
%ssort([H|T], [M|Sorted]) :- min([, M), ssort(L, Sorted).

halves([A, B], [A], [B]).
halves([A, B, C], [A, B], [C]).
halves([LeftHead|Tail], [LeftHead|LeftTail], Right) :-
	append(TailInit, [RightLast], Tail),
	append(RightInit, [RightLast], Right),
	halves(TailInit, LeftTail, RightInit).

%halves(List, Left, Pivot, Right) :- append(Left, [Pivot|Right], List).


msort([A], [A]).
msort([A, B], [A, B]) :- A < B.
msort([A, B], [B, A]) :- A >= B.

msort(X, Y) :-
	halves(X, Left, Right),
	msort(Left, SortedLeft),
	msort(Right, SortedRight),
	merge(SortedLeft, SortedRight, Y).

head([H|_], H).
tail([_|T], T).
init(L, X) :- append(X, [_], L).
last(L, X) :- append(_, [X], L).

unique([], []).
unique([H|T], Unique) :- member(H, T), unique(T, Unique).
unique([H|T], [H|Unique]) :- unique(T, Unique).

