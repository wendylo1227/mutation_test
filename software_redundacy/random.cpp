#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    ofstream outfile("data.txt");
    srand(time(0));

    int n = 500;
    outfile << n << "\n";

    for (int i = 0; i < n; i++) {
        outfile << (rand() % 1000 + 1) << " "; // 1~1000
    }
    outfile << endl;
    outfile.close();
    cout << "data.txt 生成完成！" << endl;
    return 0;
}
