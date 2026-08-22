/* test_control_flow.qs - Tests if/elif/else, loops, match, break, continue */

let x : int = 10;
if (x > 5) {
    print("x > 5");
} elif (x == 5) {
    print("x == 5");
} else {
    print("x < 5");
}

let i : int = 0;
while (i < 3) {
    print("while", i);
    i += 1;
}

let j : int = 0;
repeat {
    print("repeat", j);
    j += 1;
} until (j >= 3);

for (let k : int = 0; k < 4; k += 1) {
    if (k == 1) {
        continue;
    }
    if (k == 3) {
        break;
    }
    print("for", k);
}

let m : int = 2;
match (m) {
    case 1:
        print("one");
        break;
    case 2:
        print("two");
        fallthrough;
    case 3:
        print("three");
        break;
    default:
        print("default");
}