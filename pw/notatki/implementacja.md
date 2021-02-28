
# Implementacje mechanizmów synchronizacji

## Pb/Pv na testandset

```
fun Pb(s):
	repeat
		nottestandset(active, s) # s = false
	until active

fun Vb(s):
	s = true
```


## P/V na testandset z aktywnym czekaniem

```
ACTIVE = false
DELAY = true

fun P(S: Int):
	active = true      # Pb(ACTIVE)
	while active: testandset(active, ACTIVE)
	if S > 0:
		S = S - 1      # proces zawołał nie musiał czekać
		ACTIVE = false # Vb(ACTIVE)
	else:
		ACTIVE = false # Vb(ACTIVE)
		delay = true   # Pb(DELAY)
		while delay: testandset(delay, DELAY)

fun V(S: Int):
	active = true      # Pb(ACTIVE)
	while active: testandset(active, ACTIVE)
	if S >= 0:
		S = S + 1      # jeśli nic nie czeka zwiększamy S
	else:
		DELAY = false  # Vb(DELAY)
	ACTIVE = false     # Vb(ACTIVE)
```


## P/V na regionach krytycznych

```
fun P(var s: Semaphore, n: Int):
	region s when s - n > 0:
		s = s - n

fun V(var s: Semaphore, n: Int):
	region s:
		s = s + n
```

## Region krytyczny na wait/signal

```
region v do S

fun region(v, S):
	wait(v-mutex)
	S()
	signal(v-mutex)
```


## Warunkowy region krytyczny na wait/signal

```
region v when B do S

fun region(v, B, S):
	var x-mutex, x-delay: BinarySemaphore = 1, 0
	var x-count, x-temp: Int = 0, 0

	wait(x-mutex)
	if not B:
		x-count += 1    # zwiększenie liczby czekających na spełnienie B
		signal(x-mutex) # przepuść kogoś do sprawdzenia B
		wait(x-delay)   # czekaj aż wychodzący z S pozwoli przetestować B
		while not B:
			x-temp += 1
			if x-temp < x-count:
				signal(x-delay)
			else:
				signal(x-mutex)
			wait(x-delay)
		x-count -= 1

	S()

	if x-count > 0:     # ktoś czeka na spełnienie B
		x-temp = 0
		signal(x-delay) # przepuść pierwszy proces czekający na testowanie B
	else:
		signal(x-mutex) # nikt nie czeka na spełnienie B
```

