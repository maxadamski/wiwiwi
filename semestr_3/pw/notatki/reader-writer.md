# Pisarze-czytelnicy

## Pisarze-czytelnicy na semaforach

```
reader_count: Int = 0
mutex, writing, lock: Semaphore
writing = reading = lock = 1

fun reader():
	P(mutex)
	reader_count += 1
	if reader_count - 1 == 0:
		P(writing)
	V(mutex)

	# read data

	P(mutex)
	reader_count -= 1
	if reader_count == 0:
		V(writing)
	V(mutex)


fun writer():
	P(lock)
	P(writing)
	# write data
	V(writing)
	V(lock)
```

## Pisarze-czytelnicy na regionach krytycznych

```
shared v = struct {
	reader_count: Int = 0
	writer_count: Int = 0
}

fun reader():
	region v:
		wait(reader_count == 0)
		reader_count += 1
	# read
	region v:
		reader_count -= 1

fun writer():
	region v:
		wait(not busy and reader_count == 0)
		writer_count += 1
		busy = true
	# write
	region v:
		writer_count += 1
		busy = false
```

## Pisarze-czytelnicy na monitorach

```
monitor ReadersWriters:
	can_read, can_write: Condition
	reader_count: Int = 0
	busy: Bool = false

	fun start_read():
		if busy: can_read.wait()  # sleep if someone is writing
		reader_count += 1
		can_read.signal()         # wake up reader
	
	fun end_read:
		reader_count -= 1
		if reader_count == 0:
			can_write.signal()    # wake writers if noone is reading
	
	fun start_write:
		if busy or reader_count != 0:
			can_write.wait()      # sleep if someone is reading
		busy = true
	
	fun end_write:
		busy = false
		if can_read.queue():
			can_read.signal()     # wake readers if they want to read
		else:
			can_write.signal()    # wake writers if noone wants to read

fun reader():
	ReadersWriters.start_read()
	# read data
	ReadersWriters.end_read()

fun writer():
	# make data
	ReadersWriters.start_write()
	# write data
	ReadersWriters.end_write()
```

