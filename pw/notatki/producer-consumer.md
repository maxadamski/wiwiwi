# Producent-konsument


## Producent-konsument na semaforach

```
empty, full, mutex: Semaphore = N, 0, 1
buffer: Item[1..N]

fun producer():
	item = produce_item()
	P(empty)
	P(mutex)
	buffer.add(item)
	V(mutex)
	V(full)

fun consumer():
	P(full)
	P(mutex)
	item = buffer.take()
	V(mutex)
	V(empty)
	consume_item(item)
```

## Producent-konsument na regionach krytycznych

```
shared v = struct {
	buffer: Item[1..N]
	count: Int = 0
}

fun producer():
	item = produce_item()
	region v if count < N:
		buffer.add(item)

fun consumer():
	region v if count > 0:
		item = buffer.take()
	consume_item(item)
```

## Producent-konsument na monitorach

```
buffer: Item[N]

monitor ProducerConsumer:
	full, empty: Condition
	count: Int = 0

	fun enter():
		if count == N: full.wait()    # block if buffer is full
		buffer.put(item)
		count += 1
		if count == 1: empty.signal() # wake consumer if buffer was empty

	fun remove():
		if count == 0: empty.wait()       # block if buffer is empty
		buffer.take(item)
		count -= 1
		if count == N - 1: full.signal()  # wake producer if buffer was full

fun producer():
	produce_item
	ProducerConsumer.enter()

fun consumer():
	ProducerConsumer.remove()
	consume_item
```

## Producent-konsument na komunikatach

```
var buffer: array[0..N]

fun producer:
	message = produce_message()
	receive(producer, empty)
	buffer.add(message)
	send(consumer, empty)

fun consumer:
	receive(consumer, empty)
	message = buffer.take()
	send(producer, empty)
	process_message(message)

fun main:
	for i in N..1:
		send(producer, empty)
	parbegin:
		producer()
		consumer()
```

