/* test_type_conversions.qs - Tests all type conversion functions */

let f : float = 3.99;
let i : int = to_int(f);       /* 3 */
let s : string = to_string(f); /* "3.99" */
let c : char = to_char(65);    /* 'A' */
let b1 : bool = to_bool(1);    /* true */
let b0 : bool = to_bool(0);    /* false */

print(i);
print(s);
print(c);
print(b1);
print(b0);
print(to_float(3));            /* 3 */