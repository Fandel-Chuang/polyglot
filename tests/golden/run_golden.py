#!/usr/bin/env python3
# 文达 金样测试（Golden Test）驱动器（CLI:Command-Line Interface）
# 用法：
#   python3 tests/golden/run_golden.py
# 约定：
#   - 测例放在 tests/golden/cases/<case_name>/
#   - 输入文件命名为 input.pg 或 input.文达（二选一）
#   - 期望输出命名为 expected.out（必需）
#   - 可选退出码 expected.exit（整数，默认 0）
# 比对策略：
#   - 输出做尾随空白裁剪，换行差异不影响判定

import subprocess
import sys
import os
import json
import difflib
from pathlib import Path

# 强制控制台使用 UTF-8，避免 Windows 控制台编码报错
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    sys.stderr.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    try:
        import codecs
        sys.stdout = codecs.getwriter('utf-8')(sys.stdout.buffer, 'replace')
        sys.stderr = codecs.getwriter('utf-8')(sys.stderr.buffer, 'replace')
    except Exception:
        pass

ROOT = Path(__file__).resolve().parents[2]
BUILD_BIN = ROOT / 'build' / 'bin'
BIN_EN = BUILD_BIN / 'polyglot'
BIN_ZH = BUILD_BIN / '文达'
CASES_DIR = ROOT / 'tests' / 'golden' / 'cases'
REPORT_DIR = ROOT / 'tests' / 'golden' / 'report'


def norm(s: str) -> str:
    # 统一为 \n，并裁剪尾随空白
    return "\n".join([line.rstrip() for line in s.replace('\r\n', '\n').replace('\r', '\n').split('\n')]).rstrip()


def resolve_exec(path: Path) -> Path:
    if path.exists():
        return path
    if os.name == 'nt':
        p = Path(str(path) + '.exe')
        if p.exists():
            return p
        # 兼容中文别名
        if path.name == '文达':
            p2 = path.with_name('wenda_cn.exe')
            if p2.exists():
                return p2
    return path


def run_case(case_dir: Path) -> dict:
    input_pg = case_dir / 'input.pg'
    input_zh = case_dir / 'input.文达'
    # 兼容两种期望文件名，优先 expected.out，其次 expected.txt（为规避部分仓库 *.out 忽略规则）
    expected_out = case_dir / 'expected.out'
    if not expected_out.exists():
        expected_out = case_dir / 'expected.txt'
    expected_exit = case_dir / 'expected.exit'

    if not expected_out.exists():
        return {
            'name': case_dir.name,
            'status': 'ERROR',
            'reason': 'expected.out 缺失'
        }

    if input_pg.exists():
        exec_path = BIN_EN
        src_path = input_pg
    elif input_zh.exists():
        exec_path = BIN_ZH
        src_path = input_zh
    else:
        return {
            'name': case_dir.name,
            'status': 'ERROR',
            'reason': '缺少 input.pg 或 input.文达'
        }

    # 解析可执行路径（兼容 .exe 与 wenda_cn.exe）
    exec_path = resolve_exec(exec_path)

    if not exec_path.exists():
        return {
            'name': case_dir.name,
            'status': 'ERROR',
            'reason': f'可执行器缺失：{exec_path}'
        }

    exp_exit = 0
    if expected_exit.exists():
        try:
            exp_exit = int(expected_exit.read_text(encoding='utf-8').strip())
        except Exception:
            return {
                'name': case_dir.name,
                'status': 'ERROR',
                'reason': 'expected.exit 不是有效整数'
            }

    # 执行
    try:
        proc = subprocess.run(
            [str(exec_path), str(src_path)],
            cwd=str(ROOT),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding='utf-8'
        )
    except Exception as e:
        return {
            'name': case_dir.name,
            'status': 'ERROR',
            'reason': f'执行失败: {e}'
        }

    got_out = norm(proc.stdout)
    got_err = norm(proc.stderr)
    got_exit = proc.returncode

    exp_out = norm(expected_out.read_text(encoding='utf-8'))

    ok = (got_exit == exp_exit) and (got_out == exp_out)

    return {
        'name': case_dir.name,
        'status': 'PASS' if ok else 'FAIL',
        'exit': got_exit,
        'stdout': got_out,
        'stderr': got_err,
        'expected_exit': exp_exit,
        'expected_stdout': exp_out,
    }


def main() -> int:
    cases = []
    if not CASES_DIR.exists():
        print(f"[golden] 未发现用例目录：{CASES_DIR}")
        return 2

    for p in sorted(CASES_DIR.iterdir()):
        if p.is_dir():
            cases.append(p)

    # 确保报告目录存在，便于在循环内输出差异/错误文件
    REPORT_DIR.mkdir(parents=True, exist_ok=True)

    results = []
    passed = 0
    failed = 0
    errors = 0

    for c in cases:
        r = run_case(c)
        results.append(r)
        status = r.get('status')
        if status == 'PASS':
            passed += 1
            print(f"[PASS] {r['name']}")
        elif status == 'FAIL':
            failed += 1
            print(f"[FAIL] {r['name']}")
            exp = r.get('expected_stdout', '')
            got = r.get('stdout', '')
            # 控制台输出简要信息
            print('  --- expected ---')
            print(exp)
            print('  --- got ---')
            print(got)
            print(f"  exit: got {r.get('exit')} expected {r.get('expected_exit')}")
            # 生成差异文件，供 CI 工件下载
            diff_text = ''.join(difflib.unified_diff(
                exp.splitlines(True),
                got.splitlines(True),
                fromfile='expected',
                tofile='got',
                lineterm=''
            ))
            (REPORT_DIR / f"{r['name']}.diff.txt").write_text(diff_text, encoding='utf-8')
        else:
            errors += 1
            reason = r.get('reason', '')
            print(f"[ERROR] {r['name']}: {reason}")
            err_payload = f"reason: {reason}\n\nstdout:\n{r.get('stdout','')}\n\nstderr:\n{r.get('stderr','')}\n"
            (REPORT_DIR / f"{r['name']}.error.txt").write_text(err_payload, encoding='utf-8')

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    (REPORT_DIR / 'summary.json').write_text(
        json.dumps({'total': len(results), 'passed': passed, 'failed': failed, 'errors': errors, 'results': results}, ensure_ascii=False, indent=2),
        encoding='utf-8'
    )

    print(f"\n[golden] total={len(results)} passed={passed} failed={failed} errors={errors}")
    return 0 if (failed == 0 and errors == 0) else 1


if __name__ == '__main__':
    sys.exit(main())