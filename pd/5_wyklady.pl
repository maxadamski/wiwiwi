
%
% 3. Listy, operatory i operacje arytmetyczne
%

% konkatenacja list

append_([], B, B).
append_([H | T], B, [H | C]) :- append_(T, B, C).

% dekompozycja list:
% ?- append_(A, B, [a,b,c]).
% A = [], B = [a,b,c] ;
% A = [a], B = [b,c] ;
% ...
% A = [a,b,c], B = []

% szukanie podlist:
% ?- append_(Prefix, [b | Suffix], [a,b,c,d]).
% Prefix = [a], Suffix = [c,d]

% szukanie otoczenia:
% ?- append_(_, [Before, b, After | _], [a,b,c,d]).
% Before = a, After = c.

% usuwanie podlisty:
% ?- append_(X, [z,z,z | _], [a,b,z,c,z,z,z,u,v,w])
% X = [a,b,z,c]

% sprawdzanie przynależności:
% ?- append_(_, [a | _], [a,b,c,d]).
% true.

% dodawanie do listy

add_(X, L, [X | L]).

% usuwanie z listy (jednego) elementu

del_(X, [X | L], L).
del_(X, [H | T], [H | Del]) :- del_(X, T, Del).

% wstawienie elementu do listy
% ?- del_(a, X, [z,z]).
% X = [a,z,z] ;
% X = [z,a,z] ;
% X = [z,z,a]

insert_(Element, L, X) :- del_(Element, X, L).

% czy lista zawiera podlistę

% sublist_(S, L) :- append_(Left, Right, L), append_(S, Aux, Right).
%
%             <-------Right------->
% <---Left--> <---S----> <--Aux--->
% <---------------L--------------->
%

sublist_(S, L) :- append_(_, Right, L), append_(S, _, Right).

% permutacja listy

% operacja zapętla się po wygenerowaniu wszystkich permutacji
permutation_([], []).
permutation_([H | T], X) :- permutation_(T, Perm), insert_(H, Perm, X).

% operatory

% ?- op(600, xfx, ma).
% |: piotr ma auto.
% ?- X ma auto.
% X = piotr.

% arytmetyka

plus1(A, B, C) :- C is A + B.
plus2(A, B, C) :- C = A + B.
plus3(A, B, C) :- C =:= A + B.

% długość listy

length_([], 0).
length_([_ | T], L) :- length_(T, Length), L is 1 + Length.

%
% 4. Odcięcia
%

%
% 5. Metapredykaty
%

% var, nonvar, atom
% integer, real
% atomic = atom or integer/real

plus_safe(A, B, C) :- atomic(A),  atomic(B), C is A + B.

% not in swipl!
% ?- atom([]).
% Yes

atom_count(_, [], 0).
atom_count(A, [B | T], N) :- atom(B), A = B, !, atom_count(A, T, M), N is 1 + M.
atom_count(A, [_ | T], N) :- atom_count(A, T, N).

% arytmograf



% podstawienia

substitute_list(_, [], _, []).
substitute_list(Old, [H | T], New, [NewH | NewT]) :-
	substitute(Old, H, New, NewH),
	substitute_list(Old, T, New, NewT).

substitute(Old, Old, New, New) :- !.
%substitute(_, Term, _, Term) :- atomic(Term), !.
substitute(Old, OldTerm, New, NewTerm) :-
	OldTerm =.. [Fun | T],
	substitute_list(Old, T, New, NewT),
	NewTerm =.. [Fun | NewT].

% Term =.. [Functor | Args]
% functor(Term, Functor, Arity)
% arg(N, Term, Arg)
% name(Atomic, AciiList).

% assert(C) - dodaj do bazy
% asserta(C) - dodaj na początek bazy
% assertz(C) - dodaj na koniec bazy
% retract(C) - usuń z bazy

% !
% fail
% true
% not(P)
% call(C)
% repeat

age(bob, 7).
age(ann, 5).
age(ann, 5).
age(pat, 8).
age(tom, 5).

% bagof(Group, Term(..., Group, ...), List) <-> groupby
% setof(Group, Term(..., Group, ...), List) <-> groupby.map(set)

% ?- bagof(Age, age(Name, Age), L).
% Name = ann, L = [5, 5] ;
% Name = bob, L = [7] ;
% Name = pat, L = [8] ;
% Name = tom, L = [5].
%
% ?- setof(Name/Age, age(Name, Age), L).
% L = [ann/5, bob/7, pat/8, tom/5].
% 
% ?- bagof(Name/Age, age(Name, Age), L).
% L = [bob/7, ann/5, ann/5, pat/8, tom/5].

% 6. I/O

% Open & Close

% change/close/identify input stream
% see(filename) / seen / seeing(filename)
%
% change/close/identify output stream
% tell(filename) / told / telling(filename)
%
% no error if see/tell and filename is already open

% Read & Write

% get(NonWhitespaceByte)
% get(AnyByte)
% get(-1) -> EOF
%
% put(AnyByte)
%
% read(Term)
% read(end_of_file) -> EOF
% write(Term)
%
% read(Stream, Term)
% write(Stream, Term)
%
% append(filename)
% tab(N) -> write N spaces
% nl -> write newline
%

read_list(L) :- read(X), read_list_p(X, L).
read_list_p(stop, []) :- !.
read_list_p(X, [X | T]) :- read_list(T).

% consult(filename)
% reconsult(filename)
