import subprocess
import os
import sys
import re
import csv

# ------------------------
# 路徑設定
# ------------------------
if getattr(sys, 'frozen', False):
    SCRIPT_DIR = os.path.dirname(sys.executable)
else:
    SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

MUTANTS_DIR = os.path.join(SCRIPT_DIR, "mutants(acc time)")
REPORT_FILE = os.path.join(SCRIPT_DIR, "report.csv")
DATA_FILE = os.path.join(SCRIPT_DIR, "data.txt") # 確保 C++ 讀得到 data.txt

# 正則：抓取新的輸出格式 "Verification Cost: xxxx ns"
TIME_PATTERN = re.compile(r"Verification Cost:\s+(\d+)\s+ns")

def compile_mutant(source_path, output_path):
    result = subprocess.run(
        ["g++", "-O2", "-std=c++17", source_path, "-o", output_path],
        capture_output=True,
        text=True
    )
    return result.returncode == 0, result.stderr

def run_mutant_with_mode(exe_path, mode_number):
    """
    執行 mutant 並輸入指定的 mode (1~6)
    """
    try:
        # 準備輸入資料：將模式編號轉換為字串 (例如 "3\n")
        input_str = f"{mode_number}\n"
        
        result = subprocess.run(
            [exe_path],
            input=input_str,           # 直接用字串當作 stdin 輸入
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=5,                 # 設定 5 秒超時
            cwd=SCRIPT_DIR             # 重要：確保 C++ 能在當前目錄讀到 data.txt
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
    if not os.path.exists(DATA_FILE):
        print(f"Error: {DATA_FILE} not found! C++ program needs it.")
        return

    mutant_files = [
        f for f in os.listdir(MUTANTS_DIR)
        if f.startswith("mutant_") and f.endswith(".cpp")
    ]
    mutant_files.sort()

    # CSV 表頭
    csv_headers = ["Mutant_File", "Mode_1_Seq", "Mode_2_Sample", "Mode_3_Odd", "Mode_4_Even", "Mode_5_HeadTail", "Mode_6_Middle"]
    csv_rows = []

    print(f"\n=== Running {len(mutant_files)} Mutants against 6 Modes ===\n")

    for file in mutant_files:
        print(f"Processing {file} ... ", end="", flush=True)
        source_path = os.path.join(MUTANTS_DIR, file)
        exe_name = os.path.join(SCRIPT_DIR, f"temp_{file.replace('.cpp', '')}.exe")

        # --- 編譯 ---
        ok, err = compile_mutant(source_path, exe_name)
        if not ok:
            print("❌ Compile Error")
            continue

        row_data = [file] # CSV 的第一欄：檔名
        
        # --- 針對該變異體，跑 6 種模式 ---
        for mode in range(1, 7):
            output = run_mutant_with_mode(exe_name, mode)
            exec_time = extract_time(output)
            
            if exec_time is not None:
                row_data.append(exec_time)
            else:
                row_data.append(-1) # -1 代表失敗或超時

        csv_rows.append(row_data)
        print("✔ Done")

        # --- 清除 exe ---
        if os.path.exists(exe_name):
            os.remove(exe_name)

    # --- 寫入 CSV ---
    print("\n=== Saving Report ===")
    try:
        with open(REPORT_FILE, "w", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            writer.writerow(csv_headers)
            writer.writerows(csv_rows)
        print(f"Report saved to {REPORT_FILE}")
    except Exception as e:
        print(f"Error saving file: {e}")

if __name__ == "__main__":
    main()