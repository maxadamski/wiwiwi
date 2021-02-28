#include <iostream>
using namespace std;

constexpr int fib(int n, int a = 0, int b = 1) {
	if (n <= 0) return b;
	return fib(n - 1, b, a + b);
}

constexpr bool is_prime(int n, int i) {
	if (n <= 1) return false;
	if (i == 1) return true;
	if (n % i == 0) return false;
	return is_prime(n, i - 1);
}

constexpr bool prime(int n, int k = 2) {
	if (n <= 0) return k;
	return prime(n - is_prime(k, k - 1), k + 1);
}

template <int N>
struct Fib {
	static constexpr int value = prime(N);
};

int main() {
	cout << prime(0) << "\n";
	cout << prime(1) << "\n";
	cout << prime(2) << "\n";
	cout << prime(3) << "\n";
	cout << prime(4) << "\n";
	cout << prime(5) << "\n";
	cout << prime(6) << "\n";
	cout << prime(7) << "\n";
	cout << prime(8) << "\n";
	cout << prime(9) << "\n";
	cout << prime(10) << "\n";
	cout << prime(11) << "\n";
	return 0;
}
