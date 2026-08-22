/* fibonacci.qs - Print first 10 Fibonacci numbers */

func fib(n: int) -> int {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

print("First 10 Fibonacci numbers:");
for (let i : int = 0; i < 10; i += 1) {
    print(fib(i));
}