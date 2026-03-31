# mutation_test

Improve software quality through Mutation Testing, enhance toolchain reliability, and implement a software redundancy mechanism for automatic fault tolerance.

---

## 1. Mutant Generation & Reduction

Study how to reduce the number of mutants by merging operator rules while maintaining a high Mutation Score (MS).

- `generate/Project.cpp` (main generator program)
- Modify mutation logic in `Project.cpp` to generate optimized mutants from the original program (e.g., QuickSort)

---

## 2. Automated Execution & Analysis

Automatically compile and execute a large number of mutants, then compare outputs with expected results to calculate MS.

- `run_mutants/run_mutants.py`  
  Batch compile and execute all `.cpp` files in `mutants/`, and store outputs in `output/`

- `run_mutants/compare_output.py`  
  Compare outputs with `expected_output.txt`, count **Killed** (Wrong Output + Timeout) and **Survived**, and generate MS reports

- **Core Metric**  
  `MS = (Killed Mutants / Total Mutants) × 100%`

---

## 3. Generator Reliability Test

Test the testing tool itself to improve generator reliability.

- Apply mutation testing to the generator and observe whether the produced mutants differ from the original version  
- `batch_operation/mutants_runner.py`  
  Run benchmarks on mutated generators  
- `batch_compare/mutant_name_diff_default.py`  
  Compare source code differences to determine whether generator faults are detected  

---

## 4. Software Redundancy

Implement a runtime detection mechanism. When the main algorithm fails due to bugs, the system automatically switches to a backup algorithm.

- `software_redundancy/sort.cpp` includes 6 acceptance test modes

---
