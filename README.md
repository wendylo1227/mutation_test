# mutation_test
透過Mutation Testing提升軟體的品質、工具鏈的可靠性，並實作Software Redundancy機制以達成系統自動容錯。

## 1. Mutant Generation & Reduction (變異產生與縮減)
研究如何透過合併算子規則減少變異體數量，同時維持高變異分數 (MS)。
- generate/Project.cpp (Generator 主程式)。
- 在 Project.cpp 中修改變異邏輯，針對原始程式（如 QuickSort）產出精簡後的 Mutants。

## 2. Automated Execution & Analysis (自動化執行與評估)
自動化編譯、執行大量變異體，並與預期結果比對以計算 MS。
- run_mutants/run_mutants.py: 自動化批次編譯 mutants/ 下的所有 .cpp，並記錄輸出至 output/。
- run_mutants/compare_output.py: 比對 output/ 與 expected_output.txt，統計 Killed（包含 Wrong Output 與 Timeout）與 Survived 數量，產出 MS 報告。
核心指標：MS =(Killed Mutants\Total Mutants)*100 %。

## 3. Generator Reliability Test (工具鏈自我驗證)
測試測試工具，提升Generator品質:
對 Generator 進行變異，觀察其產出的 Mutants 是否與原始版本有異。
- batch_operation/mutants_runner.py: 在變異過的 Generator 上執行 Benchmark。
- batch_compare/mutant_name_diff_default.py: 比對源碼內容差異，判斷 Generator 的錯誤是否被偵測。

## 4. Software Redundancy (系統容錯應用)
實作 Runtime 偵測機制。當主演算法因 Bug 失效時，系統能自動切換至備援演算法。
- software_redundancy/sort.cpp 包含 6 種 Acceptance Test 模式。
