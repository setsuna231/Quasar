/* control_flow.qs - Demonstrate loops and conditionals */

let score : int = 85;

if (score >= 90) {
    print("Grade: A");
} elif (score >= 80) {
    print("Grade: B");
} else {
    print("Grade: C");
}

print("While loop:");
let i : int = 0;
while (i < 3) {
    print(i);
    i += 1;
}

print("For loop:");
for (let j : int = 0; j < 3; j += 1) {
    print(j);
}

print("Match statement:");
let option : int = 2;
match (option) {
    case 1:
        print("Option 1 selected");
        break;
    case 2:
        print("Option 2 selected");
        break;
    default:
        print("Default option");
}