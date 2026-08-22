/* functions.qs - Demonstrate user-defined functions */

func add(a: int, b: int) -> int {
    return a + b;
}

func greet(name: string) -> string {
    return "Hello, " + name;
}

func is_even(n: int) -> bool {
    return n % 2 == 0;
}

print("add(3,4) =", add(3, 4));
print("greet(\"Quasar\") =", greet("Quasar"));
print("is_even(10) =", is_even(10));
print("is_even(7) =", is_even(7));