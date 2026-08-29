# Quasar

**A statically typed, compiled programming language that transpiles to C.**

Quasar combines the clarity of high-level syntax with the performance of native compilation. It is built from scratch in C and designed for developers who want a pragmatic, extensible language without sacrificing control.

---

## Current Status

Quasar v0.20.2 is a **stable language core**. All fundamental features are implemented and pass a comprehensive regression suite under `-Wall -Wextra -Werror`. The compiler is ready for experimentation, learning, and building real programs.

---

## Features

- **Static typing** with `int`, `float`, `string`, `char`, `bool`, and `void`
- **Variables** via `let`, including multi-variable declarations
- **Control flow**:
  - `if` / `elif` / `else`
  - `while`, `repeat...until`, `for`
  - `break`, `continue`
  - `match` with `fallthrough`
- **Functions**:
  - `func` keyword
  - Parameters with type annotations
  - Return types, including `void`
  - Recursion support
- **Input/output**:
  - `print()` with multiple arguments
  - `input()` for user input
- **String operations**:
  - Concatenation using `+`
  - Repetition using `*`
  - Equality with `==` and `!=`
- **Type conversion functions**:
  - `to_int()`, `to_float()`, `to_string()`, `to_char()`, `to_bool()`
- **Arithmetic**:
  - `+`, `-`, `*`, `/`, `%`
  - Floor division `//`
  - Exponentiation `**`
- **Relational and logical operators**:
  - `==`, `!=`, `<`, `>`, `<=`, `>=`
  - `&&`, `||`, `!`
- **Compound assignment and increment/decrement**:
  - `+=`, `-=`, `*=`, `/=`, `%=`, `**=`, `//=`
  - `++`, `--` (prefix and postfix)

---

## Getting Started

### Prerequisites

- A C compiler (GCC or MinGW recommended)
- Windows PowerShell (for the provided build commands) or a Unix-like shell

### Building the Compiler

Clone the repository and build `quasar`:

```powershell
gcc -O2 -Wall -Wextra -std=c99 -Isrc src/lexer/lexer.c src/parser/ast.c src/parser/parser.c src/codegen/codegen.c src/symtab/symtab.c src/main.c -o quasar.exe
```

### Compiling and Running a Quasar Program

Create a file named `hello.qs`:

```qs
func main() -> void {
    print("Hello, Quasar!");
}
```

Compile it:

```powershell
quasar hello.qs -o hello.exe
```

Run it:

```powershell
.\hello.exe
```

Output:

```
Hello, Quasar!
```

> **Note:** `main` is optional. If omitted, the compiler treats top-level statements as the entry point.

---

## Language at a Glance

### Variables & Types

```qs
let age : int = 25;
let height : float = 5.9;
let name : string = "Vedhashiva";
let initial : char = 'V';
let is_student : bool = true;
```

### Functions

```qs
func add(a: int, b: int) -> int {
    return a + b;
}

func greet(name: string) -> void {
    print("Hello,", name);
}

let sum : int = add(3, 4);
greet("Quasar");
```

### Control Flow

```qs
let score : int = 85;

if (score >= 90) {
    print("A");
} elif (score >= 80) {
    print("B");
} else {
    print("C");
}

for (let i : int = 0; i < 5; i += 1) {
    if (i == 2) continue;
    print(i);
}
```

### Strings

```qs
let first : string = "Hello";
let second : string = "World";
let message : string = first + " " + second;  // "Hello World"
let repeat : string = "Ha" * 3;              // "HaHaHa"

if (first == second) {
    print("Same");
}
```

### Type Conversions

```qs
let pi : float = 3.14159;
let pi_int : int = to_int(pi);       // 3
let pi_str : string = to_string(pi); // "3.14159"
```

---

## Documentation

Full language reference and tutorials will be available in the `docs/` directory. For now, the source code and included test suite serve as the primary reference.

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

---

## Acknowledgments

Inspired by a love of systems programming and a dislike of outdated academic C.