/* test_functions.qs - Tests function calls, recursion, bool returns */

func add(a: int, b: int) -> int {
    return a + b;
}

func factorial(n: int) -> int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

func greet(name: string) -> string {
    return "Hello, " + name;
}

func is_even(n: int) -> bool {
    return n % 2 == 0;
}

print(add(3, 4));        /* 7 */
print(factorial(5));     /* 120 */
print(greet("Quasar"));  /* Hello, Quasar */
print(is_even(4));       /* true */
print(is_even(5));       /* false */