
# testandset

## testandset implementation (atomic)

```
fun testandset(a, b):
	*a = *b
	*b = true

fun nottestandset(a, b):
	*a = *b
	*b = false
```

## mutex with testandset

```
active = false

fun process1:
	loop:
		block = true
		while block:
			testandset(&block, &active)

		// critical section

		active = false

fun process2:
	loop:
		block = true
		while block:
			testandset(&block, &active)

		// critical section

		active = false

process1 and process2
```

# Semaphores

## Dijkstra's P and V

```
fun P(S):
	if S > 0:
		S -= 0
	else:
		(sleep process)

fun V(S):
	if (process is waiting):
		(awake process)
	else:
		S += 1
```

## Mutex with P and V

```
active = 1

fun process1:
	P(active)
	// critical section
	V(active)

.
.
.

process1() and ... and processN()
```


```
wait: BinarySemaphore = 0
lock: BinarySemaphore = 1

function P(var S: Int, n: Int):
	Pb(lock)
	S -= n
	if S <= 0:
		Vb(lock)
		Pb(wait)
	else
		S -= n
		Vb(lock)

function V(var S: Int, n: Int):
	Pb(lock)
	S += n
	if S < 0:
		Vb(wait)
	Vb(lock)

test: Int = 
```


```
mostek: Semaphore = K
statek: Semaphore = N
statek_lock: BinarySemaphore = 1

mostek_osoby = 0
statek_osoby = 0

function pasazer():
	while true:
		P(mostek)

		P(statek)

		sleep(random)

function kapitan():
	while true:
		sleep(godzina)


```
