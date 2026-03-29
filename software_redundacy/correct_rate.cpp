#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>
#include <functional>
#include <string>
#include <fstream>

using namespace std;

// =========================================================
// 1. 資料生成器 (Data Generator)
// =========================================================

vector<int> generateRandomData(int size) {
    vector<int> data(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(-10000, 10000);
    for(int &x : data) x = dis(gen);
    return data;
}

vector<int> generateSimilarData(int size) {
    vector<int> data(size, 100); // 大部分是 100
    if (size > 3) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, size - 1);
        
        data[dis(gen)] = 50;
        data[dis(gen)] = 150;
        data[dis(gen)] = 50; 
    }
    return data;
}

vector<int> generateExtremeData(int size) {
    vector<int> data(size, 0); 
    if (size >= 4) {
        data[0] = 2147483647;
        data[1] = 2147483647;
        data[2] = -2147483648;
        data[3] = -2147483648;
        random_device rd;
        mt19937 gen(rd());
        shuffle(data.begin(), data.end(), gen);
    }
    return data;
}

// =========================================================
// 2. 檢測模式 (Verifiers)
// =========================================================

bool isSorted(const vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i - 1] > arr[i]) return false;
    }
    return true;
}

bool isSortedSample(const vector<int>& arr, int sampleCount = 500) {
    if (arr.size() <= 1) return true;
    int n = arr.size();
    
    for (int s = 0; s < sampleCount; s++) {
        int i = rand() % (n - 1);
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

bool isSortedOddIndex(const vector<int>& arr) {
    if (arr.size() <= 1) return true;
    for (size_t i = 1; i < arr.size(); i += 2) {
        if (arr[i - 1] > arr[i]) return false;
    }
    return true;
}

bool isSortedEvenIndex(const vector<int>& arr) {
    if (arr.size() <= 2) return true;
    for (size_t i = 2; i < arr.size(); i += 2) {
        if (arr[i - 1] > arr[i]) return false;
    }
    return true;
}

bool isSortedHeadTailSample(const vector<int>& arr, int sampleCount = 500) {
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

bool isSortedMiddleSample(const vector<int>& arr, int sampleCount = 500) {
    int n = arr.size();
    if (n <= 2) return true;
    int start = n / 3;
    int end = (2 * n) / 3;
    if (end >= n - 1) end = n - 2;
    for (int s = 0; s < sampleCount; s++) {
        int i = start + (rand() % (end - start));
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

struct Verifier {
    string name;
    function<bool(const vector<int>&)> func;
};

// =========================================================
// 3. Mutants
// =========================================================

void quickSort_Mutant1(vector<int>& arr, int left, int right) {
    if (left != right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant1(arr, left, pivotIndex - 1);
    quickSort_Mutant1(arr, pivotIndex + 1, right);
}

void quickSort_Mutant4(vector<int>& arr, int left, int right) {
    if (left < right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant4(arr, left, pivotIndex - 1);
    quickSort_Mutant4(arr, pivotIndex + 1, right);
}

void quickSort_Mutant5(vector<int>& arr, int left, int right) {
    if (left <= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant5(arr, left, pivotIndex - 1);
    quickSort_Mutant5(arr, pivotIndex + 1, right);
}

void quickSort_Mutant7(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j > right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant7(arr, left, pivotIndex - 1);
    quickSort_Mutant7(arr, pivotIndex + 1, right);
}

void quickSort_Mutant8(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j >= right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant8(arr, left, pivotIndex - 1);
    quickSort_Mutant8(arr, pivotIndex + 1, right);
}

void quickSort_Mutant9(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j == right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant9(arr, left, pivotIndex - 1);
    quickSort_Mutant9(arr, pivotIndex + 1, right);
}

void quickSort_Mutant11(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] != pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant11(arr, left, pivotIndex - 1);
    quickSort_Mutant11(arr, pivotIndex + 1, right);
}

void quickSort_Mutant12(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] > pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant12(arr, left, pivotIndex - 1);
    quickSort_Mutant12(arr, pivotIndex + 1, right);
}

void quickSort_Mutant13(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] >= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant13(arr, left, pivotIndex - 1);
    quickSort_Mutant13(arr, pivotIndex + 1, right);
}

void quickSort_Mutant15(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] == pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant15(arr, left, pivotIndex - 1);
    quickSort_Mutant15(arr, pivotIndex + 1, right);
}

void quickSort_Mutant19(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j+=2) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant19(arr, left, pivotIndex - 1);
    quickSort_Mutant19(arr, pivotIndex + 1, right);
}

void quickSort_Mutant20(vector<int>& arr, int left, int right) {
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
    quickSort_Mutant20(arr, left, pivotIndex - 1);
    quickSort_Mutant20(arr, pivotIndex + 1, right);
}

void quickSort_Mutant21(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j+=4) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant21(arr, left, pivotIndex - 1);
    quickSort_Mutant21(arr, pivotIndex + 1, right);
}

void quickSort_Mutant22(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j+=5) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant22(arr, left, pivotIndex - 1);
    quickSort_Mutant22(arr, pivotIndex + 1, right);
}

void quickSort_Mutant43(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    bool pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    int pivotIndex = i + 1;
    swap(arr[pivotIndex], arr[right]);

    // recursive calls
    quickSort_Mutant43(arr, left, pivotIndex - 1);
    quickSort_Mutant43(arr, pivotIndex + 1, right);
}

// =========================================================
// 4. 主實驗流程 (修改版)
// =========================================================
int main() {
    srand(time(nullptr)); 
    const int DATA_SIZE = 1000; // ★建議：改小一點，不然 Mutant 11 跑很久
    
    ofstream csvFile("mutation_results_strict.csv");
    if (!csvFile.is_open()) return 1;
    csvFile << "\xEF\xBB\xBF"; // UTF-8 BOM

    // 定義 Mutants
    struct Mutant { string name; void (*func)(vector<int>&, int, int); };
    vector<Mutant> mutants = {
        {"Mutant 1", quickSort_Mutant1},
        {"Mutant 4", quickSort_Mutant4},
        {"Mutant 5", quickSort_Mutant5},
        {"Mutant 7", quickSort_Mutant7},
        {"Mutant 8", quickSort_Mutant8},
        {"Mutant 9", quickSort_Mutant9},
        {"Mutant 11", quickSort_Mutant11},
        {"Mutant 12", quickSort_Mutant12},
        {"Mutant 13", quickSort_Mutant13},
        {"Mutant 15", quickSort_Mutant15},
        {"Mutant 19", quickSort_Mutant19},
        {"Mutant 20", quickSort_Mutant20},
        {"Mutant 21", quickSort_Mutant21},
        {"Mutant 22", quickSort_Mutant22},
        {"Mutant 43", quickSort_Mutant43}
    };

    // 定義 Verifiers
    vector<Verifier> verifiers = {
        {"Full-Seq", isSorted}, 
        {"Sample", [](const vector<int>& a){ return isSortedSample(a, 500); }},
        {"Odd-Idx", isSortedOddIndex},
        {"Even-Idx", isSortedEvenIndex},
        {"Head-Tail", [](const vector<int>& a){ return isSortedHeadTailSample(a, 500); }},
        {"Middle", [](const vector<int>& a){ return isSortedMiddleSample(a, 500); }}
    };

    csvFile << "Mutant,Data Type";
    for(const auto& v : verifiers) csvFile << "," << v.name;
    csvFile << endl;

    cout << "Starting Experiments (Filtering out lucky cases)..." << endl;

    // 定義資料產生器的結構
    typedef vector<int> (*GeneratorFunc)(int);
    struct DataSetDef { string name; GeneratorFunc func; int count; };
    
    vector<DataSetDef> dataDefs = {
        {"Random", generateRandomData, 500},
        {"Similar", generateSimilarData, 250}, // 現在這組資料變難了
        {"Extreme", generateExtremeData, 250}
    };

    for (const auto& mut : mutants) {
        cout << "Running " << mut.name << "..." << endl;

        for (const auto& dDef : dataDefs) {
            
            vector<int> detectedCounts(verifiers.size(), 0);
            int validErrorCases = 0; 
            int retryCount = 0; 
            const int MAX_RETRIES = 50000; // 防止無限迴圈

            // ★ 核心邏輯：一定要湊滿 count 個「失敗案例」
            while (validErrorCases < dDef.count) {
                // 1. 生成與執行
                vector<int> originalArr = dDef.func(DATA_SIZE);
                vector<int> testArr = originalArr;
                mut.func(testArr, 0, DATA_SIZE - 1);

                // 2.過濾倖存者：如果它運氣好排對了，這筆不算
                if (isSorted(testArr)) {
                    retryCount++;
                    if (retryCount > MAX_RETRIES) {
                        // 如果試了幾萬次都排對，代表這個 Mutant 對這種資料無效
                        // 直接跳出，避免程式當掉
                        break; 
                    }
                    continue; 
                }

                // 3. 程式到這裡，代表 testArr 確定是亂的
                validErrorCases++;

                // 4. 讓各個檢測器去抓
                // 因為已經確認沒排好，Full-Seq (verifiers[0]) 一定會抓到
                for (size_t v = 0; v < verifiers.size(); ++v) {
                    if (!verifiers[v].func(testArr)) {
                        detectedCounts[v]++;
                    }
                }
            }

            // 輸出結果
            csvFile << mut.name << "," << dDef.name;
            for (int count : detectedCounts) {
                // 分母是 validErrorCases，如果 validErrorCases 為 0 (例如重試過多)，則顯示 0
                double rate = (validErrorCases > 0) ? (double)count / validErrorCases * 100.0 : 0.0;
                csvFile << "," << fixed << setprecision(2) << rate; 
            }
            csvFile << endl;
        }
    }

    csvFile.close();
    cout << "\nDone! Check 'mutation_results_strict.csv'." << endl;
    
    return 0;
}