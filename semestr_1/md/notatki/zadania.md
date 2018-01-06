---
title: Matematyka Dyskretna CW
author: Max Adamski
theme: EastLansing
colortheme: rose
fontsize: 10pt
---

# Zadania

### Lekcja 1 - Logika

1. Czy wyrazenie to zdanie logiczne, jesli tak, oblicz
  1. $\forall_{x \in R}x^2 = x$
  1. $\exists_{x \in R}x^2 = x$
  1. $x^2 = x$
1. Zdanie odwrotne i preciwstawne
  1. $\forall_{x \in R \backslash \{0\}}(x > 0 \to x^2 > 0)$
1. $p \to q \equiv 0$, oblicz $p \land q,\ p \oplus q,\ p \lor q,\ \neg p \lor q,\ \neg (p \land \neg q),\ \neg q \to \neg p,\ q \to p$
1. Wyraz $\neg, \lor, \land$ za pomoca kreski Sheffera
1. Czy zdanie jest tautologia
  1. $((p \lor q) \land r) \to (p \land \neg q)$
  1. $((p \to q) \land \neg q) \to \neg p$

### Lekcja 2 - Relacje

1. Jakie wlasnosci ma relacja $R \subseteq S \times S,\ S = \{0,1,2,3\}$
  1. $(m,n) \in R \iff m + n = 3$
  1. $(n,m) \in R \iff n \leq m$
1. Jaka relacja jest rownoleglosc prostych
1. Jakie wlasnosci ma relacja pusta $\varnothing \in S \times S$
1. A, B nie sa puste, $RZUT: a \times b \to a, RZUT(a, b) = a$
  1. Czy $RZUT$ jest na
  1. Czy $RZUT$ jest roznowartosciowa
1. Znajdz, funkcje odwrotne dla $f : R \to R$
  1. $f(x) = 2x + 3$
  1. $f(x) = \sqrt[3]{x} + 7$

### Lekcja 2 - Relacje

5. $S = \{1, 2, 3, 4, 5\},\ T = \{a, b, c, d\}$
  1. Czy istnieje funkcja roznowartosciowa $S \to T, S na T, T \to S, T na S$
  1. Czy istnieje odwzorowanie wzajemnie jednoznaczne $S w T lub T w S$
1. Rozwaz $\log{x}, x^2, \sqrt{x}, \frac{1}{x}$
  1. Znajdz dziedzine w $R$
  1. Ktore sa wzajemnie odwrotne
  1. Dla ktorych par zlozenie jest przemienne
1. $SUMA(m,n) = m + n, N \times N w N$
  1. Czy $SUMA$ jest na
  1. Czy $SUMA$ jest roznowartosciowa

### Lekcja 3 - Asymptotyka

1. Znajdz najmniejsza $k$, ze $f(n) = O(n^k)$
1. Sprawdz czy oszacowanie jest poprawne
  1. $2^{n-1} = O(2^n)$
  1. $(n + 1)^2 = O(n^2)$
  1. $40^n = O(2^n)$
  1. $(2n)! = O(n!)$
  1. $(2n)! = O((n!)^2)$
1. Korzystajac z hierarhii funkcji dokonaj oszacowania

### Lekcja 4 - Zliczanie

1. Ile jest drog w grafie
1. Ile jest anagramow
  1. FLORYDA
  2. MISSISSIPPI
1. Na ile sposobow mozna posadzic 6 osob przy okraglym stole
1. Na ile sposobow mozna posadzic 5 osob przy prostym stole
  1. Aby A i B siedzialy obok siebie
  1. Aby A i B nie siedzialy obok siebie
1. Na ile sposobow mozna wylosowac ze wracaniem sekwencje 10 kart
  1. Aby pierwsza karta sie nie powtorzyla
  1. Aby pierwsza karta sie powtorzyla

### Lekcja 4 - Zliczanie

6. Na ile sposobow mozna przejsc z $A=(2,1)$ do $B=(7,4)$ w minimalnej liczbie krokow
1. Na ile sposobow mozna przejsc z $A=(0,0)$ do $B=(7,4)$ w minimalnej liczbie krokow nie przechodzac po $XYZ$, $X=(2,2), Y=(4,2), Z=(4, 3)$
1. Na ile sposobow mozna ulozyc 12 ksiazek z 20 na polce
1. Na ile sposobow mozna podzielic 12 ksiazek na 4 dzieci
1. Na ile sposobow mozna podzielic 7 jablek, 6 truskawek na 4 dzieci, aby kazdy dostal co najmniej 1 jablko
1. Znajdz calkowitoliczbowe rozwiazania $x_1+x_2+x_3+x_4=32, x_i \geq 0$
  1. $x_i \geq 0$
  1. $x_i > 0$
  1. $x_i > 0 \land 25 \geq x$

### Lekcja 5 - Indukcja i rekurencja

Todo

### Lekcja 6 - Indukcja i liczby specjalne

1. Ile slow o dlugosci $n$ nad alfabetem $\Sigma = \{ a, b \}$ nizawierajacych $aa$
1. Udowodnij, ze zbior potegowy zbioru n-elementowego ma $2^n$ elementow
1. Udowodnij, ze $\forall_{ n \in \mathbb{N} }H_{2^n} \leq 1 + n$
1. Udowodnij, ze $\forall_{ n \in \mathbb{N} }F_n \leq (\frac{5}{3})^n$
1. Niech $A = \{ 1, 2, 3, 4, 5, 6, 7 \}$, $B = \{ v, w, x, y, z \}$. Ile jest funkcji $f : A \to B$
  1. Gdy $f(A) = \{ v, x \}$
  1. $|f(A)| = 2$
1. Na ile sposobow mozna ulozyc 4 elementy w 3 pudelkach
  1. Pudelka nie moga byc puste
  1. Pudelka moga byc puste
1. Na ile sposobow 5 osob mozna posadzic przy 3 okraglych stolach

### Wzory dodatkowe

#### Liczby Stirlinga

- $S(n, 1) = 1, gdy\ n \geq 1$
- $S(n, 2) = 2^{n - 1} - 1, gdy\ n \geq 1$

### Lekcja 7 - Liczby całkowite

1. Zapisz liczbe 6137 w systemie 8
1. Znajdz najwiekszy wspolny dzielnik (gcd) liczb 250 i 111. Zapisz go jako kombinaje liniowa tych liczb. Podaj ich najmniejsza wspolna wielokrotnosc (lcm)
1. Udowodnij, ze $8n + 3$ i $3n + 2$ sa wzglednie pierwsze
  1. $\forall_{ n \in Z^+ } gcd(8n + 3, 3n + 2) = 1$ 
1. Rozwiaz rownanie diofantycze $6x+15y=30,\ x, y \in Z$
  1. Rownanie diofantyczne $ax+by=c$ ma rozwiazanie gdy $gcd(a, b) | c$
  1. Przedstaw $gcd$ jako kombinacje liniowa i sparametryzuj ja
1. Ile jest dodatnich dzielnikow liczby $n \in Z^+ \land n > 1$
  1. Ilosc dzielnikow to $(k_1+1) \cdot (k_2+1) \cdot … \cdot (k_r+1)$
  1. np. $29 338 848 000 = 2^8 3^5 3^3 7^3 11^1q
1. Student analizuje procedure w C srednio w 6 minut a w assembrerze srednio w 10 minut ile procedur zanalizuje dokladnie w 104 minuty?

### Lekcja 8 - Teoria grafów

1. Dany jest graf nieskierowany G = (V, E)
  1. Ile spojnych podgrafow G zawiera 4 wierzcholki i cykl?
  1. Narysuj podgraf indukowany zbiorem wierzcholkow V = { … }
  1. Ile istnieje podgafow rozpinajacych grafu G?
  1. Ile istnieje spojnych podgafow rozpinajacych grafu G?
  1. Ile istnieje podgafow rozpinajacych grafu G indukowanych zbiorem wierzchoolkoow V = { … }?
1. Niech G bedzie grafem nieskierowanym G = (V, E) bez petli wlasnych
  1. Ile krawedzi zawiera dopelnienie grafu G?
1. Ile wierzcholkow i krawedzi zawiera K_4,7


### Lekcja 9 - Zliczanie

> Diagram Venna, Zasada wlaczania i wylaczania, Liczba nieporzadkow, Zasada szufladkowa

1. W pewnej instytucji pracuje wylacznie znajace co najmniej 1 jezyk obcy: 6 ang, 6 niem, 7 franc, 4 ang + niem, 3 niem + franc, 2 franc + ang, 1 zna 3 jezyki
  1. Ile osob pracuje w firmie?
  1. Ile osob zna tylko 1 jezyk?
1. Podczas wywiadowki nauczyciel informuje rodzicow, ze sposrod 30 uczniow w klasie do kolek naukowych zapisalo sie: mat 19, fiz: 13, bio: 18, chem: 11
  1. Jezeli uczen nie zapisuje sie do > 2 kolek, a kolka nie maja wiecej niz 5 wspolnych czlonkow, to czy nauczyciel mowi prawde?
1. Jakie jest prawdopodobienstwo, ze w _sekwencji_ 8 rzutow kostka pojawia sie wszystkie liczby?
1. Na ile sposobow mozna ulozyc klocki z 26 literami alfabetu aby nie pojawilo sie slowo KOT, RAZ, LUD, PIES?
1. Ile liczb w zbiorze T = {1000, 1001, …, 9999} zawiera co najmniej 9 cyfre 0, 1 i 2?

### Lekcja 9 - Zliczanie

6. Na ile sposobow 2 osoby moga przeczytac 7 ksiazek w 7 dni czytajac 1 dziennie?
1. Czy mozna zmiescic 500 000 plikow o nazwach ciag 4 liter (26 liter) w jednym katalogu?
1. Ile trzeba zaprosic osob aby mialy te same inicjaly
1. Udowodnij, ze wsrod 9 nieujemnych liczb calkowitych ktorych suma wynosi 90 istnieja:
  1. 3 liczby ktorych suma wynosi co najmniej 30
1. Pokaż, ze poniższa figura nie może być narysowana linią ciągłą bez przechodzenia pewnych fragmentów kilka razy.

### Lekcja 10 - Transwersale

> Algorytm transwersali, Tw. Halla, Skojarzenia w grafie (1), Tw. Landaua (2, 3)

1. Przepisz! (Znajdź transwersalę)
1. Który z podanych ciągów może być wektorem wyników turnieju z udziałem 6-ciu graczy? 
  1. 4, 4, 4, 2, 1, 1 (n = 6) [nie]
  1. 5, 4, 4, 1, 1, 0 (n = 6) [nie]
  1. 5, 3, 3, 2, 1, 1 (n = 6) [tak]
1. W turnieju z udziałem n graczy ile jest mażliwych wyników takich, że
  1. Żadni 2 gracze nie uzyskali tego samego wyniku
  1. 2 gracze uzyskali ten sam wynik a pozostałe osoby różne
  1. Wszyscy gracze poza 1 zajęli egzekwo 2 miejsce

### Twierdzenie Landaua

1. War. kon.: liczba zwycięztw = liczba pojedynków
1. War. dst.: \forall r \geq 2 (liczba zwycięztw r zawodników \geq \select r 2
1. w war. dst. wystarczy sprawdzić podciągi r najmniejszych wyników

