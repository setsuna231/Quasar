/* type_conversions.qs - Demonstrate type conversion functions */

let pi : float = 3.14159;
let pi_int : int = to_int(pi);
let pi_str : string = to_string(pi);
let initial : char = to_char(65);
let flag : bool = to_bool(1);

print("to_int(3.14159) =", pi_int);
print("to_string(3.14159) =", pi_str);
print("to_char(65) =", initial);
print("to_bool(1) =", flag);