import os
import re

# ===== 路徑設定 =====
OUTPUT_DIR = "output"
EXPECTED_FILE = "expected_output.txt"
REPORT_DIR = "report"
KILLED_REPORT = os.path.join(REPORT_DIR, "killed.txt")

# ===== 建立 report 資料夾 =====
os.makedirs(REPORT_DIR, exist_ok=True)

# ===== 檢查必要檔案 =====
if not os.path.exists(OUTPUT_DIR):
    print(f"找不到資料夾：{OUTPUT_DIR}")
    exit(1)

if not os.path.exists(EXPECTED_FILE):
    print(f"找不到檔案：{EXPECTED_FILE}")
    exit(1)

# ===== 讀取 expected output =====
with open(EXPECTED_FILE, "r", encoding="utf-8") as f:
    expected_content = f.read()

# ===== 找出所有 data_x.txt =====
pattern = re.compile(r"data_(\d+)\.txt$")
data_files = []

for filename in os.listdir(OUTPUT_DIR):
    match = pattern.match(filename)
    if match:
        number = int(match.group(1))
        data_files.append((number, filename))

data_files.sort(key=lambda x: x[0])

if not data_files:
    print("output 資料夾內找不到任何 data_x.txt")
    exit(1)

# ===== 統計 =====
killed = 0
survived = 0

killed_list = []
timeout_count = 0
wrong_count = 0

# ===== 比較 =====
for number, filename in data_files:
    file_path = os.path.join(OUTPUT_DIR, filename)

    with open(file_path, "r", encoding="utf-8") as f:
        data_content = f.read()

    mutant_name = f"mutant_{number}"

    # ===== 判斷 timeout =====
    if "=== Runtime Timeout ===" in data_content:
        killed += 1
        timeout_count += 1
        killed_list.append(f"{mutant_name} (TIMEOUT)")

    # ===== 判斷正確輸出 =====
    elif data_content == expected_content:
        survived += 1

    # ===== 其他錯誤（wrong output / runtime error）=====
    else:
        killed += 1
        wrong_count += 1
        killed_list.append(f"{mutant_name} (WRONG OUTPUT)")

# ===== 計算分數 =====
total = len(data_files)
mutation_score = (killed / total) * 100 if total > 0 else 0

# ===== 輸出 killed.txt =====
with open(KILLED_REPORT, "w", encoding="utf-8") as f:
    f.write("Killed mutants:\n")
    for mutant in killed_list:
        f.write(mutant + "\n")

    f.write("\n===== Summary =====\n")
    f.write(f"Killed total: {killed}\n")
    f.write(f"  - Timeout: {timeout_count}\n")
    f.write(f"  - Wrong Output: {wrong_count}\n")
    f.write(f"Survived total: {survived}\n")
    f.write(f"Total: {total}\n")
    f.write(f"Mutation Score: {mutation_score:.2f}%\n")

# ===== 顯示 =====
print("比較完成！")
print(f"Killed total: {killed}")
print(f"  - Timeout: {timeout_count}")
print(f"  - Wrong Output: {wrong_count}")
print(f"Survived total: {survived}")
print(f"Total: {total}")
print(f"Mutation Score: {mutation_score:.2f}%")
print(f"結果已寫入：{KILLED_REPORT}")