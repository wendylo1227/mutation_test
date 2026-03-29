#include <iostream>
using namespace std;

int main() {
    long long int a, b, c;
    int n;
    cin >> n;
    while (n--) {
        cin >> a >> b >> c;
        long long int min_votes, mid_votes;

        // 找出最小
        if (a < b && a < c) {
            min_votes = a;
        } else if (b < a && b < c) {
            min_votes = b;
        } else {
            min_votes = c;
        }

        // 找出中間值
        if ((a > b && a < c) || (a < b && a > c)) {
            mid_votes = a;
        } else if ((b > a && b < c) || (b < a && b > c)) {
            mid_votes = b;
        } else {
            mid_votes = c;
        }

        // 棄保操作
        if (a == min_votes) {
            a = 0;
        } else if (b == min_votes) {
            b = 0;
        } else {
            c = 0;
        }

        if (a == mid_votes) {
            a += min_votes;
        } else if (b == mid_votes) {
            b += min_votes;
        } else {
            c += min_votes;
        }

        // 決定當選者
        if (a > b && a > c) {
            cout << 'A' << endl;
        } else if (b > a && b > c) {
            cout << 'B' << endl;
        } else {
            cout << 'C' << endl;
        }
    }
    return 0;
}
