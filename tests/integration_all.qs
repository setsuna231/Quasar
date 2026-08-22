/* integration_all.qs - Combines all major features to catch interaction bugs */

func add(a: int, b: int) -> int {
    return a + b;
}

func multiply(a: int, b: int) -> int {
    return a * b;
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

let total : int = add(3, 4) * multiply(2, 5);
print("Total:", total);        /* 70 */

let fact : int = factorial(6);
print("Factorial 6:", fact);   /* 720 */

let msg : string = greet("Quasar");
print(msg);                    /* Hello, Quasar */

if (is_even(10)) {
    print("10 is even");
} else {
    print("10 is odd");
}

let count : int = 0;
for (let i : int = 0; i < 10; i += 1) {
    if (i % 2 == 0) {
        count += 1;
    }
}
print("Even count:", count);   /* 5 */

let names : string = "A" + "B" * 3 + "C";
print("String mix:", names);   /* ABBBC */