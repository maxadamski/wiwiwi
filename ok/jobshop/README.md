# Optymalizacja Kombinatoryczna: Job-shop

## Kompilacja i testy

```sh
# Na Linuxie
make build test time
```

Po zbudowaniu program ma ścieżkę się w `out/jobshop`

Czasy i miary jakości są zapisywane w `out/bench.csv`

## Podejścia

- Greedy Shortest Remaining Time First (GR-SRTF)
- Monte Carlo Tree Search (MCTS)

## Instrukcja

```
jobshop OPCJE

    -f|--file FILE
        wczytaj instancję z pliku FILE

    -m|--method METHOD
        użyj metody METHOD do rozwiązania problemu

        METHOD := greedy

    -t|--type TYPE
        parsuj instancje typu TYPE

        TYPE := beasley | tailard

    -j|--job-limit N
        wczytaj tylko N pierwszych zadań instancji

    -h|--help
        wyświetla pomoc

    -d|--debug
        wyświetla komunikaty przydatne do debugowania na stderr

    -b|--benchmark
        wyświetla czas który zajęło rozwiązywanie instanji na stdout
        czas mierzony w nanosekundach (linux)

    -v|--visual
        wizualizuje rozwiązanie w trybie debugowania
        może się nieprawidłowo wyświetlać, gdy liczba zadań jest większa od 36
        
    -g|--graph
        wyświetla listę sąsiedztwa dla każdego wierzchołka (taska), oraz czas konstrukcji grafu

    -c|--convert
        wyświetla instancję w formacie beasley na stdout
        przydatne gdy jako FILE jest podana instancja tailard
```

```
script/test OPCJE

    -b|--bench FILE
        dla każdej metody, dla każdej instancji wypisz czas wykonania i jakość
        rozwiązania do pliku FILE, w formacie CSV

    -t|--test
        dla każdej metody, dla każdej instancji użyj sprawdzarki
        w res/checker do zweryfikowania odpowiedzi

    -m|--method METHOD
        dodaj metodę do listy metod, które będą użyte dla każdej instancji
```

## Autorzy

Max Adamski, Daniel Cieśliński
