import subprocess
import os
import sys
import re
import statistics

# ------------------------
# 路徑設定
# ------------------------
if getattr(sys, 'frozen', False):
    SCRIPT_DIR = os.path.dirname(sys.executable)
else:
    SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

MUTANTS_DIR = os.path.join(SCRIPT_DIR, "mutants")
REPORT_FILE = os.path.join(SCRIPT_DIR, "report.txt")

# 正則：抓取 "Total Execution Time: xxxx ns"
TIME_PATTERN = re.compile(r"Total Execution Time:\s+(\d+)\s+ns")

def compile_mutant(source_path, output_path):
    result = subprocess.run(
        ["g++", "-O2", "-std=c++17", source_path, "-o", output_path],
        capture_output=True,
        text=True
    )
    return result.returncode == 0, result.stderr

def run_mutant(exe_path):
    input_file = os.path.join(SCRIPT_DIR, "input.txt")
    try:
        with open(input_file, "r") as infile:
            result = subprocess.run(
                [exe_path],
                stdin=infile,              # 傳入 input.txt
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                timeout=10
            )
        return result.stdout
    except subprocess.TimeoutExpired:
        return "[Timeout]"

def extract_time(output_text):
    match = TIME_PATTERN.search(output_text)
    if match:
        return int(match.group(1))
    return None

def main():
    mutant_files = [
        f for f in os.listdir(MUTANTS_DIR)
        if f.startswith("mutant_") and f.endswith(".cpp")
    ]
    mutant_files.sort()

    all_times = []
    report_lines = []

    print("\n=== Running Mutants ===\n")

    for file in mutant_files:
        print(f"Processing {file} ...")
        source_path = os.path.join(MUTANTS_DIR, file)
        exe_name = f"temp_{file.replace('.cpp', '')}.exe"

        # --- 編譯 ---
        ok, err = compile_mutant(source_path, exe_name)
        if not ok:
            print("  ❌ Compile error, skip")
            continue

        # --- 執行 ---
        output = run_mutant(exe_name)

        # --- 擷取時間 ---
        exec_time = extract_time(output)

        if exec_time is not None:
            print(f"  ✔ Execution Time: {exec_time} ns")
            all_times.append(exec_time)
            report_lines.append(f"{file}: {exec_time} ns")
        else:
            print("  ❌ No valid time found")
            print("  Output was:")
            print(output)

        # --- 清除 exe ---
        if os.path.exists(exe_name):
            os.remove(exe_name)

    # --- Summary ---
    print("\n=== Summary ===")
    report_lines.append("\n=== Summary ===")
    if all_times:
        avg_time = statistics.mean(all_times)
        print(f"Mutants Executed: {len(all_times)}")
        print(f"Average Execution Time: {avg_time:.2f} ns\n")
        report_lines.append(f"Mutants Executed: {len(all_times)}")
        report_lines.append(f"Average Execution Time: {avg_time:.2f} ns\n")
    else:
        print("No valid execution time collected.")
        report_lines.append("No valid execution time collected.")

    # --- 寫入 report.txt ---
    with open(REPORT_FILE, "w", encoding="utf-8") as f:
        for line in report_lines:
            f.write(line + "\n")

    print(f"Report saved to {REPORT_FILE}")

if __name__ == "__main__":
    main()
