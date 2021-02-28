# Palacze tytoniu - Koncepcja


## Idea realizacji

Palacze składają się z dwóch procesów.
Jeden odbiera pieniądze od kupujących i dodaje je do swojego portfela.
Drugi nabywa składniki, wysyła pieniądze do sprzedawcy, a na końcu pali.
Ja interpretuję pierwszy proces jako konto bankowe,
gdzie są wpłacane pieniądze po wygranej aukcji.

Agent ustawia ceny składników, oraz wybudza pierwszego palacza,
który ma wystarczająco pieniędzy w porfelu do kupna składników.
Jeżeli nikt nie mógł kupić, to agent na nowo ustawia ceny.

Myślę, że ciekawe jest to, że moja implementacja działa dla N palaczy.
Wystarczy zmienić wartość stałej `SMOKERS`.


## Struktury danych

`M + L` - liczba semaforów
`S` - liczba palaczy

### Kolejka komunikatów `msgd`

Wymiana gotówki odbywa się przez wysyłanie i odbieranie komunikatów.

Komunikat `Message`:

| pole        | typ    | opis             |
|-------------|--------|------------------|
| `mtype`     | `long` | `id+1` odbiorcy  |
| `mvalue[0]` | `int`  | kwota            |
| `mvalue[1]` | `int`  | `id` nadawcy     |

Ponieważ palacze są indeksowani od zera, do `mtype` musimy dodać `1`.
W przeciwnym przypadku zerowy palacz odbierze każdy komunikat,
a my chcemy, żeby odbierał tylko te adresowane do niego.

Aby ułatwić korzystanie z kolejki komunikatów, zdefiniowałem makra `SEND` i `RECV`.

### Pamięć współdzielona `shmd`

Tablica `shared` składa się z dwóch obszarów intów o wielkości `S`.
Każdy obszar jest przesunięty o `offset` względem początku `shared`.

| nazwa    | offset | i-ta wartość              |
|----------|--------|---------------------------|
| `wallet` | `0*S`  | ilość pieniędzy palacza i |
| `price`  | `1*S`  | cena składnika i          |

### Semafory `semd`

Tworzymy `M + L` semaforów binarnych, gdzie `M = 2; L = 2*S`.
Semafory są inicjalizowane na `init`.

| semafor         | opis                                       | ilość | init |
|-----------------|--------------------------------------------|-------|------|
| `agent_mutex`   | semafor usypiający agenta                  | 1     | 0    |
| `consumer_lock` | semafory usypiające i-tego palacza         | S     | 0    |
| `market_mutex`  | semafor chroniący `price`                  | 1     | 1    |
| `wallet_lock`   | semafory chroniące `wallet[i]`             | S     | 1    |

Aby ułatwić korzystanie z semaforów zdefiniowałem makra `P` i `V` (binarne),
oraz `enum Semaphore` zawierający offsety dla wybranych semaforów.


## Mechanizm synchronizacji oraz sposób komunikacji

Użyte semafory i komunikaty zostały opisany w sekcji powyżej.
Dokładny mechanizm opisałem komentarzami w pliku `palacze.cc`.
Myślę, że taki sposób jest łatwiejszy,
niż rozpisywanie się na parę paragrafów,
oraz ułatwia pełne zrozumienie kodu.

