# Zadanie 1

x = (-1)^s * m * 2^c

s: znak
m: mantysa
c: cecha / wykladnik

# Zadanie 2

[1,2); cecha; 1

# Zadanie 3

7/4 = 0.174	= 0 0111 1100 01100100010110100001110
-24		= 1 1000 0011 10000000000000000000000
29/32 = 0.90625 = 0 0111 1110 11010000000000000000000

# Zadanie 4

0.8	= 0 0111 1110 (1001)
-2	= 1 1000 0000 (0)
1.75	= 0 0111 1111 11(0)
-5	= 1 1000 0001 01(0)
0.3	= 0 0111 1101 (0011)
9	= 0 1000 0010 001(0)

# Zadanie 5

1/2*b_1 + 1/4*b_2 + 1/8*b_3 + ... + 1/2^n*b_n

# Zadanie 6

5/8	= 10100
13/32	= 01101
11/16	= 10110
3/4	= 11000
22/32	= 10110
3/16	= 00110

# Zadanie 7

e^x = lim_{n->\inf} (1 + \frac{x}{n})^n = (1/0! + 1/1! + 1/2! + ...)^x

# Zadanie 8

```c
int nwt(int n, int k) { 
	return k == 0 || k == n ? 1 : nwt(n-1, k-1) + nwt(n-1, k);
}
```

# Zadanie 9

```c
double P(int n, double x) {
	if (n == 0) return 0;
	if (n == 1) return x;
	double dn = (double) n;
	return (2*dn+1)/(dn+1)*x*P(n-1, x) - dn/(dn+1)*P(n-2, x);
}
```

# Zadanie 10

```c
double H(int n, double x) {
	if (n == 0) return 1;
	if (n == 1) return 2*x;
	return 2*x*H(n-1, x) - 2*n*H(n-2, x);
}
```
