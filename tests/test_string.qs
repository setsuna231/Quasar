/* test_strings.qs - Tests string concatenation, repetition, and equality */

let s1 : string = "Hello";
let s2 : string = "World";

let concat : string = s1 + " " + s2;
print(concat);                 /* Hello World */

let repeated : string = "Ha" * 3;
print(repeated);               /* HaHaHa */

print(s1 == s2);               /* false */
print(s1 != s2);               /* true */
print("test" == "test");       /* true */
print("test" != "tset");       /* true */