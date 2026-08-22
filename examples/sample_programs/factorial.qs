/* factorial.qs - Calculate factorial using recursion */

func factorial(n: int) -> int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

let num : int = 6;
print("Factorial of", num, "is", factorial(num));