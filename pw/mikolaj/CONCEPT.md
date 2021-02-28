# Święty Mikołaj - Koncepcja


## Idea realizacji

Renifery i elfy to dwa typy agenta. Wydarzenie to rozdawanie prezentów lub konsultacje.

Agent śpi, a jak się obudzi to idzie do poczekalni, próbuje obudzić mikołaja i czeka,
aż mikołaj rozpocznie wydarzenie, w zależności od typu agenta.
Następnie agent czeka na zakończenie wydarzenia, żegna mikołaja i idzie spać.

Mikołaj śpi i czeka, aż w poczekalni jest wystarczająca liczba agentów danego typu (renifery mają oczywiście priorytet).
Mikołaj rozpoczyna wydarzenie mówiąc o tym czekającym agentom danego typu.
Po zakończeniu wydarzenia Mikołaj mówi agentom, żeby sobie poszli i czeka, aż tak się stanie.
Następnie mikołaj idzie spać.

Jest jeszcze dodatkowy wątek wypisujący status mikołaja i agentów na wyjście.

Cieszę się, że udało mi się scalić procedury reniferów i elfów.
Dzięki temu można z niewielkimi modyfikacjami powiększyć program
dla nowych typów pomocników Mikołaja, gdyby zdecydował się ich zatrudnić...


## Struktury danych

### Typy wyliczeniowe

`AgentStatus = IDLE | WAIT | BUSY` - śpi, jest w poczekalni lub pracuje
`SantaStatus = IDLE | PRESENTS | MEETING` - śpi, rozdaje prezenty lub ma konsultacje

### Zmienne współdzielone

`AgentStatus status[id]` - status agenta o identyfikatorze `id`
`SantaStatus santa_status` - status Mikołaja

### Argumenty wątku

Wątek agenta otrzymuje jako argument strukturę `AgentArgs` z:

`id` - identyfikatorem agenta, czyli indeksem tablicy `status`
`type` - typem agenta, czyli indeksem tablicy `agent_busy`, `agent_wait`, `AGENT_PROB` etc.

### Mutexy

`agent_status_mutex` - mutex chroniący statusy agentów
`santa_status_mutex` - mutex chroniący status Mikołaja

### Zmienne warunkowe

Każdy i-ty typ agenta ma zmienne warunkowe `agent_wait` i `agent_busy`.
Mikołaj ma zmienne warunkowe `santa_sleep` i `santa_done`

- `agent_wait[i]`
	- agenci śpią na tej zmiennej gdy przychodzą do poczekalni.
	- Mikołaj wybudza agentów na tej zmiennej gdy się obudzi.
- `agent_busy[i]`
	- agenci śpią na tej zmiennej gdy są w trakcie wydarzenia.
	- Mikołaj wybudza agentów na tej zmiennej gdy skończy wydarzenie.
- `santa_sleep`
	- Mikołaj śpi na tej zmiennej gdy nie ma wystarczającej ilości agentów w poczekalni
	- agenci wybudzają Mikołaja na tej zmiennej gdy przychodzą do poczekalni
- `santa_busy`
	- Mikołaj śpi na tej zmiennej gdy czeka aż wszyscy agenci wyjdą
	- agenci wybudzają Mikołaja na tej zmiennej gdy idą spać


## Mechanizm synchronizacji

Użyte mutexy i zmienne warunkowe zostały opisane w sekcji powyżej.
Tak jak w zadaniu z palaczami, dokładny mechanizm opisałem komentarzami w pliku `mikolaj.cc`.
Myślę, że taki sposób jest łatwiejszy, niż rozpisywanie się na parę paragrafów, oraz ułatwia pełne zrozumienie kodu.

