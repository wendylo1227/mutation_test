/* A program to find day of a given date */
#include <iostream>
using namespace std;

int dayofweek(int d, int m, int y) {
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    if (m < 3) {
        y -= 1;
    }
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

int main() {
    int n, day, month, year;
    cin >> n;
    while (n--) {
        cin >> day >> month >> year;
        day = dayofweek(day, month, year);
        cout << day;
    }

}