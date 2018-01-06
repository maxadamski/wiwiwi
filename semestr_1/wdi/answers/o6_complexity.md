# Zadanie 1

      17
  16      10
15  12  9    3

# Zadanie 2

takemax():

      3
  16      10
15  12  9

      16
  15      10
3   12  9
      
takemax():

      9
  15      10
3   12

      15
  12      10
3    9
   
# Zadanie 7

O(n^2)

# Zadanie 8

a. 3
b. O(MaxNode)
c.

# Zadanie 10

```c
int fib(int n) { // O(n)
	if (n < 0) return -1;
	int *F = calloc(n + 1, sizeof(int));
	F[0] = 0; F[1] = 1;
	for (int i = 2; i <= n; i += 1)
		F[i] = F[i - 1] + F[i - 2];
	int result = F[n];
	free(F);
	return result;
}
```

