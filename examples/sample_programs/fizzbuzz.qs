/* fizzbuzz.qs - Classic FizzBuzz from 1 to 20 */

for (let i : int = 1; i <= 20; i += 1) {
    if (i % 15 == 0) {
        print("FizzBuzz");
    } elif (i % 3 == 0) {
        print("Fizz");
    } elif (i % 5 == 0) {
        print("Buzz");
    } else {
        print(i);
    }
}