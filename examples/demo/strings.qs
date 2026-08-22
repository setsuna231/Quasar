/* strings.qs - Demonstrate string operations */

let first : string = "Hello";
let second : string = "World";

let combined : string = first + " " + second;
print("Concatenation:", combined);

let repeated : string = "Ha" * 3;
print("Repetition:", repeated);

print("Equality:", first == second);
print("Inequality:", first != second);