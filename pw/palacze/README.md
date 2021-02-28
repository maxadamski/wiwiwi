# Palacze tytoniu

## Instrukcja obsługi

Proszę użyć komendy `make build`, a następnie wykonać plik `a.out`.

Aby zmienić parametry programu, proszę zmienić wartości stałych w pliku `palacze.hh`.

Plik `palacze.cc` zawiera właściwy kod programu.

Plik `palacze.hh` zawiera importy, stałe i mało ciekawe funkcje/makra pomocnicze.

## Opis problemu

Zadanie oparte jest na problemie palaczy tytoniu [Wikipedia](https://en.wikipedia.org/wiki/Cigarette_smokers_problem) przy czym:

1. palacze rozliczają się między sobą za udostępniane składniki po aktualnym kursie giełdowym;
2. rola agenta sprowadza się do ustalania kursu giełdowego składników;
3. palacze nie mogą mieć debetu, więc jeśli palacza nie stać na zakup składników, musi poczekać na zmianę kursu lub przypływ gotówki za sprzedaż własnego składnika (początkowy stan konta palacza jest większy od 0);
4. palacz nabywa jednocześnie oba składniki;
5. należy użyć kolejki komunikatów do przekazywania składników między palaczami.
