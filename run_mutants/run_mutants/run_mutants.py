import os
import re
import subprocess
import shutil   # 用來刪資料夾

# ===== 路徑設定 =====
MUTANTS_DIR = "mutants"
INPUT_FILE = "input.txt"
OUTPUT_DIR = "output"
EXEC_DIR = "executables"   # 暫存編譯後的執行檔

# ===== 建立輸出資料夾 =====
os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(EXEC_DIR, exist_ok=True)

# ===== 檢查 input.txt 是否存在 =====
if not os.path.exists(INPUT_FILE):
    print(f"找不到輸入檔：{INPUT_FILE}")
    exit(1)

# ===== 找出所有 mutant_x.cpp =====
cpp_files = []
pattern = re.compile(r"mutant_(\d+)\.cpp$")

for filename in os.listdir(MUTANTS_DIR):
    match = pattern.match(filename)
    if match:
        number = int(match.group(1))
        cpp_files.append((number, filename))

# 依照編號排序
cpp_files.sort(key=lambda x: x[0])

if not cpp_files:
    print("在 mutants 資料夾中找不到任何 mutant_x.cpp 檔案")
    exit(1)

print(f"共找到 {len(cpp_files)} 個 mutant cpp 檔案")

# ===== 逐一編譯並執行 =====
for number, cpp_file in cpp_files:
    cpp_path = os.path.join(MUTANTS_DIR, cpp_file)

    exec_name = f"mutant_{number}.exe" if os.name == "nt" else f"mutant_{number}"
    exec_path = os.path.join(EXEC_DIR, exec_name)

    output_file = os.path.join(OUTPUT_DIR, f"data_{number}.txt")

    print(f"\n處理中: {cpp_file}")

    try:
        # ===== 編譯 =====
        compile_cmd = ["g++", cpp_path, "-o", exec_path]
        compile_result = subprocess.run(
            compile_cmd,
            capture_output=True,
            text=True,
            timeout=30
        )

        if compile_result.returncode != 0:
            with open(output_file, "w", encoding="utf-8") as f:
                f.write("=== Compile Error ===\n")
                f.write(compile_result.stderr.rstrip() + "\n")
            print(f"編譯失敗，錯誤已寫入 {output_file}")
            continue

        # ===== 執行 =====
        with open(INPUT_FILE, "r", encoding="utf-8") as infile:
            run_result = subprocess.run(
                [exec_path],
                stdin=infile,
                capture_output=True,
                text=True,
                timeout=30
            )

        with open(output_file, "w", encoding="utf-8") as f:
            if run_result.returncode == 0:
                f.write(run_result.stdout.rstrip() + "\n")
            else:
                f.write("=== Runtime Error ===\n")
                f.write(run_result.stderr.rstrip() + "\n")
                if run_result.stdout:
                    f.write("\n=== Partial Output ===\n")
                    f.write(run_result.stdout.rstrip() + "\n")

        print(f"執行完成，結果已存入 {output_file}")

    except subprocess.TimeoutExpired:
        with open(output_file, "w", encoding="utf-8") as f:
            f.write("=== Runtime Timeout ===\n")
            f.write("程式執行超時\n")
        print(f"執行超時，結果已寫入 {output_file}")

    except Exception as e:
        with open(output_file, "w", encoding="utf-8") as f:
            f.write("=== Exception ===\n")
            f.write(str(e).rstrip() + "\n")
        print(f"發生例外，錯誤已寫入 {output_file}")

    finally:
        # ===== 刪除執行檔 =====
        if os.path.exists(exec_path):
            try:
                os.remove(exec_path)
                print(f"已刪除執行檔: {exec_name}")
            except Exception as e:
                print(f"刪除執行檔失敗: {e}")

# ===== 最後刪除 executables 資料夾 =====
try:
    if os.path.exists(EXEC_DIR):
        shutil.rmtree(EXEC_DIR)
        print("\n已刪除 executables 資料夾")
except Exception as e:
    print(f"刪除 executables 資料夾失敗: {e}")

print("\n全部處理完成！")