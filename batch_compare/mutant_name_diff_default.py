# -*- coding: utf-8 -*-
"""
smart_batch_diff_autoroot.py
以腳本所在資料夾為 root：
  baseline = ./reference_mutants
  parent   = ./all_mutants  (內含多個 mutants_* 子資料夾)

功能：
- 比對每個 mutants_* 與 baseline 是否相同
- 判斷整個資料夾是否完全相同 (added=0, missing=0, different=0)
- 在終端機逐一列出各資料夾狀態
- 最下面印出 survuved 清單，最後一行才顯示數量
- 輸出三個報告：
  1. reports/_summary.json   → JSON 總結 (summary 放最上面)
  2. reports/survuved.txt   → 相同資料夾清單 + 數量
  3. reports/killed.txt   → 不同資料夾清單 + 數量
"""

import argparse, difflib, json, re, sys, shutil
from pathlib import Path
from typing import List, Dict

# ---------- 小工具 ----------
def is_hidden(p: Path) -> bool:
    return p.name.startswith('.')

def natural_key(s: str):
    def try_int(t): return int(t) if t.isdigit() else t.lower()
    import re as _re
    return [try_int(x) for x in _re.split(r'(\d+)', s)]

def gather_files(root: Path, exts: List[str], recursive: bool) -> List[Path]:
    it = root.rglob('*') if recursive else root.iterdir()
    out = []
    for p in it:
        if p.is_file() and not is_hidden(p):
            if not exts or p.suffix.lower() in exts:
                out.append(p)
    return out

def read_text(path: Path) -> str:
    return path.read_text(encoding='utf-8', errors='ignore')

# ---------- 差異判定 ----------
_COMMENT_SL = re.compile(r'//.*?$', re.MULTILINE)
_COMMENT_ML = re.compile(r'/\*.*?\*/', re.DOTALL)

def strip_comments(text: str) -> str:
    text = _COMMENT_SL.sub('', text)
    text = _COMMENT_ML.sub('', text)
    return text

def normalize_for_decision(text: str, mode: str) -> str:
    if mode == 'strip':
        t = strip_comments(text)
        lines = t.splitlines()
        norm = [' '.join(line.strip().split()) for line in lines]
        return '\n'.join(norm)
    return text

def normalize_for_line_compare(text: str, mode: str) -> List[str]:
    if mode == 'strip':
        t = strip_comments(text)
        return [' '.join(line.strip().split()) for line in t.splitlines()]
    return text.splitlines()

def diff_line_numbers(a_lines: List[str], b_lines: List[str]) -> List[int]:
    sm = difflib.SequenceMatcher(a=a_lines, b=b_lines, autojunk=False)
    nums = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            continue
        if i1 == i2:
            nums.append(i1 + 1)   # 插入：標示插入點之後
        else:
            nums.extend(range(i1 + 1, i2 + 1))  # 置換/刪除
    return sorted(set(nums))

# ---------- 單一目標資料夾比對 ----------
def compare_one(base_dir: Path, targ_dir: Path, exts, mode, recursive) -> Dict:
    base_files = gather_files(base_dir, exts, recursive)
    targ_files = gather_files(targ_dir, exts, recursive)

    base_map = {p.name: p for p in base_files}
    targ_map = {p.name: p for p in targ_files}

    base_names = set(base_map.keys())
    targ_names = set(targ_map.keys())

    missing = sorted(base_names - targ_names, key=natural_key)
    added   = sorted(targ_names - base_names, key=natural_key)
    common  = sorted(base_names & targ_names, key=natural_key)

    different = []
    diff_lines_map: Dict[str, List[int]] = {}

    for name in common:
        b_raw = read_text(base_map[name])
        t_raw = read_text(targ_map[name])

        b_decide = normalize_for_decision(b_raw, mode)
        t_decide = normalize_for_decision(t_raw, mode)

        if b_decide != t_decide:
            different.append(name)
            a_lines = normalize_for_line_compare(b_raw, mode)
            b_lines = normalize_for_line_compare(t_raw, mode)
            lines = diff_line_numbers(a_lines, b_lines)
            diff_lines_map[name] = lines

    return {
        "baseline_files": len(base_files),
        "target_files": len(targ_files),
        "added": added,
        "missing": missing,
        "different": [{"name": n, "lines": diff_lines_map.get(n, [])} for n in sorted(different, key=natural_key)]
    }

# ---------- 主程式 ----------
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mode', choices=['raw','strip'], default='raw')
    ap.add_argument('--ext', default='.cpp', help='副檔名，逗號分隔')
    ap.add_argument('--recursive', action='store_true')
    ap.add_argument('--pattern', default='mutant_', help='all_mutants 下子資料夾前綴')
    ap.add_argument('--report-dir', default='reports')
    args = ap.parse_args()

    script_dir = Path(__file__).resolve().parent
    root = script_dir
    base_dir = root / 'reference_mutants'
    parent_dir = root / 'all_mutants'

    if not base_dir.exists() or not parent_dir.exists():
        print("[ERROR] 找不到 baseline 或 parent，請確認 reference_mutants 與 all_mutants 在同一層。")
        sys.exit(1)

    exts = [e.strip().lower() for e in args.ext.split(',') if e.strip()]
    targets = [p for p in sorted(parent_dir.iterdir(), key=lambda x: natural_key(x.name))
               if p.is_dir() and p.name.startswith(args.pattern)]

    report_dir = (root / args.report_dir)

    if report_dir.exists():
        shutil.rmtree(report_dir)
    report_dir.mkdir(parents=True, exist_ok=True)

    print("=== smart_batch_diff_autoroot ===")
    print(f"Baseline : {base_dir}")
    print(f"Parent   : {parent_dir} (prefix={args.pattern})")
    print(f"Mode={args.mode} | Ext={exts or 'ALL'} | Recursive={args.recursive}")
    print(f"Total targets: {len(targets)}\n")

    survuved_targets: List[str] = []
    killed_targets: List[str] = []
    summary = {}

    for targ in targets:
        result = compare_one(base_dir, targ, exts, args.mode, args.recursive)
        added = len(result["added"])
        missing = len(result["missing"])
        diff = len(result["different"])
        is_survuved = (added == 0 and missing == 0 and diff == 0)
        if is_survuved:
            survuved_targets.append(targ.name)
        else:
            killed_targets.append(targ.name)

        summary[targ.name] = {
            "added": added,
            "missing": missing,
            "different": diff,
            "is_survuved": is_survuved
        }

        status = "survuved" if is_survuved else "DIFF"
        print(f"[{targ.name}] {status} | added={added} missing={missing} diff={diff}")

        (report_dir / f"{targ.name}.json").write_text(
            json.dumps(result, ensure_ascii=False, indent=2), encoding="utf-8"
        )

    # === 終端機總結 ===
    print("\n=== Survuved mutants ===")
    if survuved_targets:
        for name in survuved_targets:
            print("  -", name)
    else:
        print("  (none)")

    print("\n=== Killed ===")
    if killed_targets:
        for name in killed_targets:
            print("  -", name)
    else:
        print("  (none)")

    # 最下面數量
    print(f"\nTotal targets     : {len(targets)}")
    print(f"Survuved mutants : {len(survuved_targets)}")
    print(f"Killed mutants : {len(killed_targets)}")

    # survuved.txt
    (report_dir / "survuved.txt").write_text(
        "count = " + str(len(survuved_targets)) + "\n" + "\n".join(survuved_targets),
        encoding="utf-8"
    )

    # killed.txt
    (report_dir / "killed.txt").write_text(
        "count = " + str(len(killed_targets)) + "\n" + "\n".join(killed_targets),
        encoding="utf-8"
    )

    # _summary.json（summary 在最上面）
    overall = {
        "targets_total": len(targets),
        "survuved_count": len(survuved_targets),
        "survuved_targets": survuved_targets,
        "different_count": len(killed_targets),
        "killed_targets": killed_targets,
        "by_target": summary
    }
    (report_dir / "_summary.json").write_text(json.dumps(overall, ensure_ascii=False, indent=2), encoding="utf-8")

    print(f"\nSummary written to: {report_dir / '_summary.json'}")
    print(f"survuved list written to: {report_dir / 'survuved.txt'}")
    print(f"Different list written to: {report_dir / 'killed.txt'}")

if __name__ == "__main__":
    main()
