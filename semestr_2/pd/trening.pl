
%
% 1 standardowe predykaty
%

% 1.1 sortowanie z kluczem

% V: value, K: key
lex_key(V, K) :- V = K.

encode(_, [], []).
encode(F, [V | T], [K/V | Encoded]) :- call(F, V, K), encode(F, T, Encoded).

decode([], []).
decode([_/V | T], [V | Decoded]) :- decode(T, Decoded).

insert(A, [], [A]).
insert(A, [B | T], [A, B | T]) :- A = AK/_, B = BK/_, AK =< BK, !.
insert(A, [B | T], [B | Insert]) :- insert(A, T, Insert).

isort([], []).
isort([H | T], Sorted) :- isort(T, Z), insert(H, Z, Sorted).
isort(F, X, Y) :- encode(F, X, U), isort(U, V), decode(V, Y).

% definicje map wartość -> klucz:
% |- relative_key(List, V, K) :- average(List, Avg), K is abs(V - Avg)
% |- add_key(Add, V, K) :- K is V + Add
%
% klucz bez kontekstu:
% ?- X = [8,4,1,6,8],
%    isort(lex_key, X, Y).
%
% przekazujemy kontekst do kluczowania:
% ?- isort(add_key(2), X, Y).
% ?- isort(relative_key(X), X, Y).

% 1.2 listy

% najmniejsza i największa wartość w liście
min_max([A], A, A).
min_max([H | T], Min, Max) :- min_max(T, MinT, MaxT), Min is min(H, MinT), Max is max(H, MaxT).

% długość listy
len([], 0).
len([_ | T], X) :- len(T, Len), X is 1 + Len.

% suma elementów listy
sum([], 0).
sum([H | T], X) :- sum(T, Sum), X is H + Sum.

% średnia elementów listy
avg(L, X) :- sum(L, Sum), len(L, Len), X is Sum / Len.

% odwrócona lista
rev([], []).
rev(L, [H | Rev]) :- append(T, [H], L), rev(T, Rev).

% indeks elementu listy
find(E, [E | _], I, I).
find(E, [H | T], I, Find) :- E \= H, J is I + 1, find(E, T, J, Find).
find(E, L, I) :- find(E, L, 1, I).

% unikalne elementy listy
unique([], []).
unique([H | T], Unique) :- unique(T, Unique), member(H, Unique), !.
unique([H | T], [H | Unique]) :- unique(T, Unique).

% różnica zbiorów
setdiff([], _, []).
setdiff([H | T], L, Diff) :- member(H, L), !, setdiff(T, L, Diff).
setdiff([H | T], L, [H | Diff]) :- setdiff(T, L, Diff).

% wycinek listy (odpowiednik L[A:B+1] w Pythonie)
slice(I, _, B, _, []) :- I > B, !.
slice(I, A, B, [_ | T], Slice) :- I < A, !, J is I + 1, slice(J, A, B, T, Slice).
slice(I, A, B, [H | T], [H | Slice]) :- J is I + 1, slice(J, A, B, T, Slice).
slice(A, B, L, Slice) :- slice(1, A, B, L, Slice).

% funkcyjny map
map(_, [], []).
map(F, [H | T], [MapH | MapT]) :- call(F, H, MapH), map(F, T, MapT).

% funkcyjny zip
zip([], [], []).
zip([AH | AT], [BH | BT], [AH/BH | Zip]) :- zip(AT, BT, Zip).

%
% 2. kolokwia
%

% 2.1 liczby kongruentne

congruent2(_, _, []).
congruent2(K, U, [H | T]) :- U is H mod K, congruent2(K, U, T).

congruent(I, L, I) :- congruent2(I, _, L), !.
congruent(I, L, N) :- I > 1, J is I - 1, congruent(J, L, N).
congruent(L, N) :- min_max(L, _, Max), congruent(Max, L, N).

% 2.2 odleglosc w alfabecie

letter(Letter, Code) :-
	find(Letter, [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z], Code).
letter_distance(A, B, N) :- letter(A, U), letter(B, V), N is abs(U - V).

very_unique(L, [], L).
very_unique(L, [Set | Sets], Unique) :- setdiff(L, Set, Z), very_unique(Z, Sets, Unique).

% 2.3 przedziały (2018)

% Macie na wejściu listę przedziałów np. [[-3,5],[2,10],[3,8],[0,7]]
% I listę liczb np. [-4,-1,2,4,5,7,8,9]
% Wybieracie dla danego przedziału liczby które do niego należą 
% [-3,5] -> [-1,2,4,5] i z tego zbioru liczba najbardziej oddalona od środka przedziału jest kluczem,tutaj środek 1 klucz 5
% Zakładam, że sortujemy przedziały.

in_range(_, [], []).
in_range(R, [H | T], [H | Select]) :- R = [A, B], H >= A, H =< B, !, in_range(R, T, Select).
in_range(R, [_ | T], Select) :- in_range(R, T, Select).

distance(Middle, V, K) :- K is abs(Middle - V).

middle([A, B], X) :- X is abs(A + B) / 2.

julia_key(Numbers, Range, Key) :-
	in_range(Range, Numbers, InRange), middle(Range, Middle),
	isort(distance(Middle), InRange, ByDistance),
	append(_, [Key], ByDistance).

julia(Numbers, Ranges, Result) :- isort(julia_key(Numbers), Ranges, Result).

% 2.4 liczby binarne (2018)

% Masz listę liczb, liczby konwertujesz na binarne, przesuwasz jedynki w prawo,
% konwertujesz z powrotem na dziesiętne i to jest klucz.
% Przykład: 20 -> 10100; jedynki w prawo: 00011;
% na dziesiętne: 3 - to jest nasz klucz.
% To samo dla pozostałych liczb w liście i sortujemy.

dec2base(B, N, [N]) :- N < B.
dec2base(B, N, D) :- N >= B, Div is N // B, Mod is N mod B,
	dec2base(B, Div, Z), append(Z, [Mod], D).

base2dec(_, [], 0, _).
base2dec(B, D, N, I) :- append(Init, [Last], D), J is I + 1, base2dec(B, Init, NT, J), N is NT + Last * B**I.
base2dec(B, D, N) :- base2dec(B, D, N, 0).

jerzyk_key(Dec, SortedDec) :- 
	dec2base(2, Dec, Bin),
	isort(default_key, Bin, SortedBin),
	base2dec(2, SortedBin, SortedDec).

jerzyk(X, Y) :- isort(jerzyk_key, X, Y).

% 2.5 rozpiętość przedziałów (2018)

% Jest lista przedziałów i trzeba wyznaczyć rozpiętość wszystkich przedziałów
% (czyli element najmniejszy ze wszystkich i największy), 
% potem zrobić listę rozpiętości każdego przedziału i obliczyć stałą szerokość 
% (rozpiętość wszystkich / ilość przedziałów) i zrobić listę odchyleń od stałej szerokości 
% (abs szerokość przedziału - stała szerokość) i to jest klucz sortowania wejściowej listy

span([A, B], Span) :- Span is abs(A - B).

lo_key([A, _], A).
hi_key([_, A], A).

sum_span(Ranges, Span) :-
	isort(lo_key, Ranges, SortedMin),
	isort(hi_key, Ranges, SortedMax),
	append(_, [[_, Max]], SortedMax),
	SortedMin = [[Min, _] | _],
	span([Min, Max], Span).

avg_span(Ranges, Span) :-
	sum_span(Ranges, Sum),
	length(Ranges, Count),
	Span is Sum / Count.

kuba_key(AverageSpan, Range, Key) :-
	span(Range, Span),
	Key is abs(Span - AverageSpan).

kuba(Ranges, Sorted) :-
	avg_span(Ranges, AvgSpan),
	isort(kuba_key(AvgSpan), Ranges, Sorted).

