#!/usr/bin/env python3
# 金样测试运行器（中文路径与中文用例名）

import subprocess
import sys
import os
import json
import difflib
import re
from pathlib import Path

# 统一控制台编码为 UTF-8
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    sys.stderr.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass

# 明细输出模式（默认开启）
明细输出 = True

根 = Path(__file__).resolve().parents[2]
构建目录 = 根 / 'build' / 'bin'
可执行_英文 = 构建目录 / 'polyglot'
可执行_中文 = 构建目录 / '文达'
用例目录 = 根 / '自动化测试' / '金样测试' / '用例'
报告目录 = 根 / '自动化测试' / '金样测试' / '报告'


def 规范化(s: str) -> str:
    # 去除 BOM
    if s.startswith('\ufeff'):
        s = s.lstrip('\ufeff')
    # 统一换行并裁剪尾随空白
    return "\n".join([行.rstrip() for 行 in s.replace('\r\n', '\n').replace('\r', '\n').split('\n')]).rstrip()


# 过滤编译横幅/日志行，仅保留“用户程序输出”
_LOG_PREFIX = re.compile(r'^(🌐|✅|🔤|🚀|📦|📝|🔍|🧠|❌|🎉|⏭️|🎯|🔨|🌳|📊|📄|🎨|⚠️)')

def 过滤日志(s: str) -> str:
    if not s:
        return s
    行 = []
    for line in s.split('\n'):
        if not _LOG_PREFIX.match(line):
            行.append(line)
    return "\n".join(行).strip()


def 解析可执行(path: Path) -> Path:
    if path.exists():
        return path
    if os.name == 'nt':
        p = Path(str(path) + '.exe')
        if p.exists():
            return p
        if path.name == '文达':
            别名 = path.with_name('wenda_cn.exe')
            if 别名.exists():
                return 别名
    return path


def 运行用例(目录: Path) -> dict:
    输入_pg = 目录 / 'input.pg'
    输入_中文 = 目录 / '输入.文达'
    期望_输出 = 目录 / '期望.输出'
    if not 期望_输出.exists():
        期望_输出 = 目录 / '期望.文本'
    期望_错误 = 目录 / '期望.错误'
    期望_退出 = 目录 / '期望.退出'

    if not 期望_输出.exists() and not 期望_错误.exists():
        return {'名称': 目录.name, '状态': '错误', '原因': '缺少 期望.输出/期望.文本 或 期望.错误'}

    if 输入_pg.exists():
        执行器 = 可执行_英文
        源 = 输入_pg
    elif 输入_中文.exists():
        执行器 = 可执行_中文
        源 = 输入_中文
    else:
        return {'名称': 目录.name, '状态': '错误', '原因': '缺少 input.pg 或 输入.文达'}

    执行器 = 解析可执行(执行器)
    if not 执行器.exists():
        return {'名称': 目录.name, '状态': '错误', '原因': f'可执行器缺失：{执行器}'}

    退出码期望 = 0
    if 期望_退出.exists():
        try:
            退出码期望 = int(规范化(期望_退出.read_text(encoding='utf-8')))
        except Exception:
            return {'名称': 目录.name, '状态': '错误', '原因': '期望.退出 不是有效整数'}

    try:
        进程 = subprocess.run([
            str(执行器), '--quiet', str(源)
        ], cwd=str(根), stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, encoding='utf-8')
    except Exception as e:
        return {'名称': 目录.name, '状态': '错误', '原因': f'执行失败: {e}'}

    实得_出 = 过滤日志(规范化(进程.stdout))
    实得_错 = 过滤日志(规范化(进程.stderr))
    实得_退 = 进程.returncode

    通过 = False
    结果 = {'名称': 目录.name, '退出码': 实得_退, 'stdout': 实得_出, 'stderr': 实得_错}

    if 期望_错误.exists():
        期望_错 = 规范化(期望_错误.read_text(encoding='utf-8'))
        通过 = (实得_退 == 退出码期望) and (实得_错 == 期望_错)
        结果['expected_stderr'] = 期望_错
    else:
        期望_出 = 规范化(期望_输出.read_text(encoding='utf-8'))
        通过 = (实得_退 == 退出码期望) and (实得_出 == 期望_出)
        结果['expected_stdout'] = 期望_出

    结果['状态'] = '通过' if 通过 else '失败'
    return 结果


def 主程序():
    if not 用例目录.exists():
        print(f"[golden] 未发现用例目录：{用例目录}")
        return 2

    报告目录.mkdir(parents=True, exist_ok=True)

    用例集 = [p for p in sorted(用例目录.iterdir()) if p.is_dir()]

    统计 = {'总数': 0, '通过': 0, '失败': 0, '错误': 0, '结果': []}

    for 例 in 用例集:
        r = 运行用例(例)
        统计['结果'].append(r)
        统计['总数'] += 1
        if r.get('状态') == '通过':
            统计['通过'] += 1
            print(f"[通过] {r['名称']}")
            if 明细输出 and r.get('stdout'):
                print(r['stdout'])
        elif r.get('状态') == '失败':
            统计['失败'] += 1
            print(f"[失败] {r['名称']}")
            # 生成差异
            预期 = r.get('expected_stdout', '') or r.get('expected_stderr', '')
            实得 = r.get('stdout', '') if 'expected_stdout' in r else r.get('stderr', '')
            差异 = ''.join(difflib.unified_diff(
                预期.splitlines(True), 实得.splitlines(True), fromfile='expected', tofile='got', lineterm=''
            ))
            (报告目录 / f"{r['名称']}.diff.txt").write_text(差异, encoding='utf-8')
            if 明细输出:
                print('  --- expected ---')
                print(预期)
                print('  --- got ---')
                print(实得)
                exp_exit = 0 if 'expected_stdout' in r else (r.get('expected_exit', 1))
                print(f"  exit: got {r.get('退出码')} expected {exp_exit}")
        else:
            统计['错误'] += 1
            print(f"[错误] {r['名称']}: {r.get('原因','')}")
            详情 = f"原因: {r.get('原因','')}\n\nstdout:\n{r.get('stdout','')}\n\nstderr:\n{r.get('stderr','')}\n"
            (报告目录 / f"{r['名称']}.error.txt").write_text(详情, encoding='utf-8')

    (报告目录 / '汇总.json').write_text(json.dumps(统计, ensure_ascii=False, indent=2), encoding='utf-8')
    print(f"\n[golden] 总数={统计['总数']} 通过={统计['通过']} 失败={统计['失败']} 错误={统计['错误']}")
    return 0 if (统计['失败'] == 0 and 统计['错误'] == 0) else 1


if __name__ == '__main__':
    sys.exit(主程序())