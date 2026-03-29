#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
批次編譯與執行 operation_mutants/ 中大量 C++ 程式，並整理輸出到 all_mutants/。

更新：
- 開始前會清空 all_mutants/。
- 開始前會刪除根目錄的 error.txt（若存在）。
- original_program.cpp（或 original_progran.cpp）會複製到 operation_mutants/。
- mutants/ 必須位於 operation_mutants/；執行時 cwd 固定為 operation_mutants/。
- 不建立 bin/；可執行檔直接在 operation_mutants/ 產生，用完即刪。
- 每支程式需要 input.txt（與本檔案同層）。
- 只在「失敗」時於 all_mutants/<程式名>/ 建立 error.txt。
- 根目錄 error.txt 只列出失敗清單（不列成功）。
- 執行結束會在終端機輸出總耗時。
"""

from pathlib import Path
import shutil
import subprocess
import sys
import time
import os
from typing import List, Tuple

# ===== 可調參數 =====
COMPILER = "g++"
CPP_STD = "c++17"
OPT_FLAGS = ["-O2"]
EXTRA_FLAGS: List[str] = []
TIMEOUT_SEC = 300
STD_SNIPPET_MAX = 1200
# ====================

def ensure_dir(p: Path):
    p.mkdir(parents=True, exist_ok=True)

def clean_dir(p: Path):
    if not p.exists():
        p.mkdir(parents=True, exist_ok=True)
        return
    for child in p.iterdir():
        if child.is_file() or child.is_symlink():
            child.unlink(missing_ok=True)
        else:
            shutil.rmtree(child, ignore_errors=True)

def write_text(p: Path, content: str):
    ensure_dir(p.parent)
    p.write_text(content, encoding="utf-8")

def find_original_source(script_dir: Path) -> Tuple[Path, str]:
    for name in ("original_program.cpp", "original_progran.cpp"):
        p = script_dir / name
        if p.exists():
            return p, name
    return None, ""

def copy_original_to_operation(script_dir: Path, op_dir: Path, notes: List[str]):
    src, used_name = find_original_source(script_dir)
    if not src:
        notes.append("警告：找不到 original_program.cpp / original_progran.cpp，已略過複製。")
        return
    dst = op_dir / used_name
    if dst.exists():
        dst.unlink()
    shutil.copy2(src, dst)
    notes.append(f"已複製 {used_name} → operation_mutants/")

def natural_key_for_mutant(p: Path):
    stem = p.stem
    parts = stem.split("_")
    try:
        num = int(parts[-1])
    except Exception:
        num = float("inf")
    return (parts[0], num, stem)

def find_targets(op_dir: Path) -> List[Path]:
    targets = set()
    targets.update(op_dir.glob("mutants_*.cpp"))
    targets.update(op_dir.glob("mutant_*.cpp"))
    return sorted(targets, key=natural_key_for_mutant)

def compile_cpp(src: Path, out_bin: Path) -> Tuple[bool, str]:
    cmd = [COMPILER, "-std=" + CPP_STD, *OPT_FLAGS, "-o", str(out_bin), str(src), *EXTRA_FLAGS]
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, cwd=str(src.parent))
    except FileNotFoundError:
        return False, f"找不到編譯器：{COMPILER}（請確認已安裝並在 PATH 中）"
    except Exception as e:
        return False, f"編譯異常：{e}"
    if proc.returncode != 0:
        return False, (proc.stderr or proc.stdout).strip()
    return True, ""

def run_binary(bin_path: Path, input_file: Path, cwd: Path) -> Tuple[bool, int, str, str]:
    try:
        with input_file.open("r", encoding="utf-8") as fin:
            proc = subprocess.run(
                [str(bin_path)],
                cwd=str(cwd),
                stdin=fin,
                capture_output=True,
                text=True,
                timeout=TIMEOUT_SEC
            )
    except subprocess.TimeoutExpired:
        return False, -1, "", f"執行逾時（>{TIMEOUT_SEC}s）"
    except Exception as e:
        return False, -1, "", f"執行異常：{e}"
    ok = (proc.returncode == 0)
    return ok, proc.returncode, proc.stdout or "", proc.stderr or ""

def move_mutants_payload(mutants_dir: Path, dest_dir: Path) -> int:
    ensure_dir(dest_dir)
    if not mutants_dir.exists():
        return 0
    count = 0
    for child in list(mutants_dir.iterdir()):
        target = dest_dir / child.name
        if target.exists():
            if target.is_file() or child.is_symlink():
                target.unlink()
            else:
                shutil.rmtree(target, ignore_errors=True)
        shutil.move(str(child), str(target))
        count += 1
    return count

def truncate(s: str, limit: int) -> str:
    if len(s) <= limit:
        return s
    head = s[:limit]
    return head + f"\n...[truncated {len(s)-limit} chars]"

def format_failure_detail(kind: str, cpp_name: str, exit_code: int = None,
                          stdout: str = "", stderr: str = "", extra: str = "") -> str:
    lines = []
    lines.append(f"{cpp_name}：{kind}")
    if extra:
        lines.append(extra.strip())
    if exit_code is not None and exit_code != -1:
        lines.append(f"退出碼：{exit_code}")
    if stdout:
        lines.append("\n== STDOUT ==")
        lines.append(truncate(stdout.strip(), STD_SNIPPET_MAX))
    if stderr:
        lines.append("\n== STDERR ==")
        lines.append(truncate(stderr.strip(), STD_SNIPPET_MAX))
    lines.append("")  # 末尾換行
    return "\n".join(lines)

def main():
    script_dir = Path(__file__).resolve().parent
    op_dir = script_dir / "operation_mutants"
    all_mutants = script_dir / "all_mutants"
    mutants_tmp = op_dir / "mutants"
    input_file = script_dir / "input.txt"
    root_error = script_dir / "error.txt"

    ensure_dir(op_dir)
    ensure_dir(all_mutants)
    ensure_dir(mutants_tmp)

    # 🔥 開始前先刪除根目錄的 error.txt
    if root_error.exists():
        root_error.unlink()

    # 🔥 開始前清空 all_mutants
    clean_dir(all_mutants)

    global_notes: List[str] = []
    copy_original_to_operation(script_dir, op_dir, global_notes)

    targets = find_targets(op_dir)
    if not targets:
        lines = ["在 operation_mutants/ 找不到 mutants_*.cpp 或 mutant_*.cpp"]
        if global_notes:
            lines.append("")
            lines.extend(global_notes)
        write_text(root_error, "\n".join(lines) + "\n")
        print(lines[0])
        return

    if not input_file.exists():
        global_notes.append("警告：缺少 input.txt（位於專案根目錄）；所有執行將失敗。")

    failed_summaries: List[str] = []
    start_ts = time.time()
    exe_suffix = ".exe" if os.name == "nt" else ""

    for idx, src in enumerate(targets, 1):
        name = src.stem
        per_out_dir = all_mutants / name
        ensure_dir(per_out_dir)

        print(f"[{idx}/{len(targets)}] 處理 {src.name}")

        clean_dir(mutants_tmp)

        bin_path = op_dir / f"{name}{exe_suffix}"
        ok_c, c_msg = compile_cpp(src, bin_path)
        if not ok_c:
            detail = format_failure_detail("編譯失敗", src.name, extra=c_msg)
            write_text(per_out_dir / "error.txt", detail)
            failed_summaries.append(f"{src.name}：編譯失敗")
            if bin_path.exists():
                bin_path.unlink(missing_ok=True)
            move_mutants_payload(mutants_tmp, per_out_dir)
            continue

        if input_file.exists():
            ok_r, code, out_s, err_s = run_binary(bin_path, input_file, cwd=op_dir)
        else:
            ok_r, code, out_s, err_s = (False, -1, "", "缺少 input.txt")

        bin_path.unlink(missing_ok=True)

        if not ok_r:
            detail = format_failure_detail("執行失敗", src.name, exit_code=code,
                                           stdout=out_s, stderr=err_s)
            write_text(per_out_dir / "error.txt", detail)
            failed_summaries.append(f"{src.name}：執行失敗（退出碼 {code}）")
            move_mutants_payload(mutants_tmp, per_out_dir)
            continue

        moved = move_mutants_payload(mutants_tmp, per_out_dir)
        if moved == 0:
            detail = format_failure_detail("無輸出", src.name,
                                           extra="程式執行完成但沒有任何輸出。")
            write_text(per_out_dir / "error.txt", detail)
            failed_summaries.append(f"{src.name}：無輸出")

    elapsed = int(time.time() - start_ts)
    mins, secs = divmod(elapsed, 60)

    summary_lines = [f"總計處理：{len(targets)} 支程式，耗時約 {mins} 分 {secs} 秒", ""]
    if failed_summaries:
        summary_lines.append("失敗清單：")
        summary_lines.extend(failed_summaries)
    else:
        summary_lines.append("全部成功（無失敗項目）。")
    if global_notes:
        summary_lines.append("")
        summary_lines.extend(global_notes)

    write_text(root_error, "\n".join(summary_lines) + "\n")

    print(f"完成。總結已寫入 ./error.txt")
    print(f"總耗時：{mins} 分 {secs} 秒")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)
