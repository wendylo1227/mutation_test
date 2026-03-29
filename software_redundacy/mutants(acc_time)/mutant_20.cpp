#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
using namespace std;

// ===================================================
// Quick Sort (with error protection)
// ===================================================
void quickSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j+=3) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort(arr, left, pivotIndex - 1);
    quickSort(arr, pivotIndex + 1, right);
}


// ===================================================
// Bubble Sort (with error protection)
// ===================================================
void bubbleSort(vector<int>& arr) {
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;

        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        if (!swapped) break;
    }
}


// ===================================================
// Insertion Sort (with error protection)
// ===================================================
void insertionSort(vector<int>& arr) {
    int n = arr.size();

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}


// ===================================================
// Full sequential check
// ===================================================
bool isSorted(const vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i - 1] > arr[i])
            return false;
    }
    return true;
}


// ===================================================
// Sampling check
// ===================================================
bool isSortedSample(const vector<int>& arr, int sampleCount = 10) {
    if (arr.size() <= 1) return true;
    int n = arr.size();

    srand(time(nullptr));

    for (int s = 0; s < sampleCount; s++) {
        int i = rand() % (n - 1);
        if (arr[i] > arr[i + 1]) {
            cout << "Error at sample index " << i
                 << ": " << arr[i] << " > " << arr[i + 1] << endl;
            return false;
        }
    }
    return true;
}


// ===================================================
// Odd index check
// ===================================================
bool isSortedOddIndex(const vector<int>& arr) {
    if (arr.size() <= 1) return true;

    for (size_t i = 1; i < arr.size(); i += 2) {
        if (arr[i - 1] > arr[i]) {
            cout << "Odd Index Error: " << (i - 1)
                 << " > " << i << endl;
            return false;
        }
    }
    return true;
}


// ===================================================
// Even index check
// ===================================================
bool isSortedEvenIndex(const vector<int>& arr) {
    if (arr.size() <= 2) return true;

    for (size_t i = 2; i < arr.size(); i += 2) {
        if (arr[i - 1] > arr[i]) {
            cout << "Even Index Error: " << (i - 1)
                 << " > " << i << endl;
            return false;
        }
    }
    return true;
}


// ===================================================
// Head-Tail sampling check
// ===================================================
bool isSortedHeadTailSample(const vector<int>& arr, int sampleCount = 5) {
    int n = arr.size();
    if (n <= 1) return true;

    for (int i = 1; i < sampleCount && i < n; i++) {
        if (arr[i - 1] > arr[i]) return false;
    }

    for (int i = n - sampleCount; i < n - 1; i++) {
        if (i < 0) continue;
        if (arr[i] > arr[i + 1]) return false;
    }

    return true;
}


// ===================================================
// Middle sampling check
// ===================================================
bool isSortedMiddleSample(const vector<int>& arr, int sampleCount = 5) {
    int n = arr.size();
    if (n <= 2) return true;

    int start = n / 3;
    int end = (2 * n) / 3;
    if (end >= n - 1) end = n - 2;

    srand(time(nullptr));

    for (int s = 0; s < sampleCount; s++) {
        int i = start + (rand() % (end - start));
        if (arr[i] > arr[i + 1]) return false;
    }

    return true;
}


// ===================================================
// 主程式
// ===================================================
int main() {
    ifstream infile("data.txt");
    if (!infile) {
        cerr << "Can't open data.txt !" << endl;
        return 1;
    }

    int n;
    infile >> n;

    vector<int> original(n);
    for (int i = 0; i < n; i++) {
        infile >> original[i];
    }
    infile.close();


    // 使用者選擇檢查模式
    int mode;
    cout << "Select verification mode:\n";
    cout << "1. Sequential check\n";
    cout << "2. Sampling check\n";
    cout << "3. Odd-index check\n";
    cout << "4. Even-index check\n";
    cout << "5. Head-Tail sampling check\n";
    cout << "6. Middle sampling check\n";
    cout << "Enter choice (1~6): ";
    cin >> mode;


    bool (*verifyFunc)(const vector<int>&) = nullptr;

    if (mode == 1) verifyFunc = isSorted;
    else if (mode == 2) verifyFunc = [](const vector<int>& arr) { return isSortedSample(arr, 20); };
    else if (mode == 3) verifyFunc = isSortedOddIndex;
    else if (mode == 4) verifyFunc = isSortedEvenIndex;
    else if (mode == 5) verifyFunc = [](const vector<int>& arr) { return isSortedHeadTailSample(arr, 5); };
    else if (mode == 6) verifyFunc = [](const vector<int>& arr) { return isSortedMiddleSample(arr, 5); };
    else {
        cerr << "Invalid mode! Default Sequential.\n";
        verifyFunc = isSorted;
    }

    // ============================
    // ★ 設定計時邏輯 (Wrapper)
    // ============================
    long long totalCheckTime = 0; // 用來累積「只花在檢測上」的時間

    //這是一個 Lambda 函式，用來包裝 verifyFunc 並計時
    auto runTimedCheck = [&](const vector<int>& arr) -> bool {
        auto start = chrono::high_resolution_clock::now();
        
        bool result = verifyFunc(arr); // 執行真正的檢測
        
        auto end = chrono::high_resolution_clock::now();
        
        // 累加時間
        totalCheckTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        return result;
    };


    vector<int> data = original;

    // ============================
    // 執行排序與檢測
    // ============================
    
    // 1. QuickSort (不計時)
    quickSort(data, 0, n - 1);

    // 檢測 (計時)
    if (!runTimedCheck(data)) {
        cout << "\nQuickSort failed\nFallback to BubbleSort\n";

        data = original;
        // 2. BubbleSort (不計時)
        bubbleSort(data);

        // 檢測 (計時)
        if (!runTimedCheck(data)) {
            cout << "\nBubbleSort failed\nFallback to InsertionSort\n";

            data = original;
            // 3. InsertionSort (不計時)
            insertionSort(data);

            // 檢測 (計時)
            if (!runTimedCheck(data)) {
                cout << "All sorting algorithms failed!" << endl;
            }
        }
    }

    // 輸出純檢測花費的時間
    cout << "\n============================================\n";
    cout << "Verification Mode: " << mode << endl;
    cout << "Verification Cost: " << totalCheckTime << " ns\n";
    cout << "============================================\n";


    // 輸出結果
    cout << "\nSorted Result: ";
    // 為了版面整潔，如果數據太多只印前20個
    int printLimit = (data.size() > 20) ? 20 : data.size();
    for (int i = 0; i < printLimit; i++) cout << data[i] << " ";
    if (data.size() > 20) cout << "...";
    cout << endl;

    return 0;
}

