# Jak zdać Programowanie Współbieżne

## Grafy przepływu procesów

### notacja and
### notacja parbegin/end
### notacja fork/join/quit

## Sieci Petriego

### elementarna
### znakowana
### etykietowana

## Algorytmy Synchronizacji

## Problem wzajemnego wykluczania

Założenia:

- Zapis i odczyt wspólnych danych jest atomowy
- Sesje krytyczne nie mają priorytetu
- Prędkości proesów są nieznane
- Proces może zostać zawieszony poza sekcją krytyczną
- Proces poza sekcją nie uniemożliwiają innym procesom wejścia do sekcji
- Procesy uzyskują dostęp do sekcji w skończonym czasie
- W każdej chwili czasu co najwyżej jeden proces jest w sekcji

### Algorytm Dekkera
### Algorytm Petersona
### Algorytm Lamporta
### Algorytm Dijkstry

```
shared flag[1..N] of (NIE, TAK, WYBRANY)
shared turn: 1..N

fun process(i: Int):
	while true:
		START:

		flag[i] = TAK
		other = turn

		while other != i:
			# jeżeli jest kogoś tura i on nie chce, to przypisujemy sobie turę
			test = flag[other]
			if test == NIE: turn = i
			other = turn

		flag[i] = WYBRANY

		for k in 1..N: if k != i:
			# jeżeli jakiś proces też jest wybrany to rezygnujemy z tury
			test = flag[k]
			if test == WYBRANY: goto START

		# sekcja krytyczna

		flag[i] = NIE

		# reszta
```



## Semafory

### Instrukcja testandset
### Sekcje krytyczne z testandset
### Semafory ogólne
### Sekcje krytyczne z semaforami
### Semafory binarne


## Regiony krytyczne i monitory

### Instrukcje wait i signal
### Inne instrukcje

### Monitory i implementacja

### Jedzący filozofowie na monitorach

### Alokacja zasobów na monitorach

```
monitor ResourceAllocation:
	x: Condition
	busy: Bool = false

	fun acquire(time: Int):
		if busy: x.wait(time) # process priority
		busy = true

	fun release():
		busy = false
		x.signal()
```

### Trzy drukarki na monitorach

```
monitor Printers:
	x: Condition
	busy: Array[1..3] of Bool

	fun acquire(priority: Int):
		printer = 0

		# wybierasz wolną drukarkę
		for i in 1..3:
			if not busy[i]:
				printer = i

		# czekaj jeżeli wszystkie drukarki zajęte
		if printer == 0:
			x.wait(priority)

		# zajmij drukarkę
		busy[printer] = true
		return printer

	fun release(printer: Int):
		# zwolnij drukarkę
		busy[printer] = false
		x.signal()

	
fun process(priority: Int):
	printer = Printers.acquire(priority)
	# drukuj
	Printers.release(printer)
```


## Wymiana komunikatów

### Operacje send i receive

## Zakleszczenie

### Definicja

Warunki konieczne

- Wzajemne wykluczenie (zasób może być przydzielony co najwyżej jednemu procesowi)
- Zachowywanie zasobu (proces oczekujący na zasób nie zwalnia swoich zasobów)
- Nieprzywłaszczalność (tylko proces może zwolnić swój zasób)
- Istnienie cyklu oczekiwań (cykl ubiegania się procesów o zasób)

Podejścia

- Systemy wolne od zakleszczenia (więcej zasobów niż procesów)
- Detekcja i odtwarzanie (gdy wykryjemy deadlock robimy rollback)
- Unikanie (unikanie przez analizę przyszłości)
- Zapobieganie (wyeliminowanie jednego z warunków koniecznych)


### Algorytm Holt'a
### Algorytm Wait-Die
### Algorytm Wound-Wait


