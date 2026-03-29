#include <iostream>
#include <vector>
#include <chrono>
using namespace std;

int main() {
    int T;
    cin >> T;  // 測資數量

    while (T--) {
        int N;
        cin >> N;

        vector<vector<double>> A(N, vector<double>(N));
        vector<vector<double>> B(N, vector<double>(N));
        vector<vector<double>> C(N, vector<double>(N, 0.0));

        // 輸入 A
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                cin >> A[i][j];

        // 輸入 B
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                cin >> B[i][j];

        auto start = chrono::high_resolution_clock::now();

        // 矩陣乘法
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                for (int k = 0; k < N; ++k)
                    C[i][j] += A[i][k] * B[k][j];

        auto end = chrono::high_resolution_clock::now();

        // 🔹 輸出結果
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j)
                cout << C[i][j] << " ";
            cout << "\n";
        }

        // 🔹 每組測資之間空一行（可選）
        if (T > 0) cout << "\n";
    }

    return 0;
}