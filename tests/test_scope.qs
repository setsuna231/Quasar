/* test_scope.qs - Tests variable scope and visibility */

let a : int = 10;
print("a =", a);

{
    let b : int = 20;
    print("b =", b);

    {
        let c : int = 30;
        print("c =", c);
    }
    /* c is out of scope here */
}

/* b is out of scope here */
print("a again =", a);