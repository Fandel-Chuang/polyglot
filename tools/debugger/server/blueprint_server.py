#!/usr/bin/env python3
# 文达调试器 - 蓝图服务端（最小原型）
# 说明：
# - 无第三方依赖，仅使用 Python 标准库
# - 提供 /graph/runtime 接口，返回静态蓝图数据（类容器/函数节点/调用边）
# - 仅绑定 127.0.0.1，支持 --port 与 --token（可选）

import json
import argparse
import socketserver
from http.server import SimpleHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse

RUNTIME_GRAPH = {
    "nodes": [
        {"id": "g_init", "kind": "global", "name": "初始化系统", "visited": 3, "hasBreakpoint": False},
        {"id": "m_player_start", "kind": "method", "class": "玩家", "name": "开始", "visited": 5, "hasBreakpoint": False, "clusterId": "c_player"},
        {"id": "m_player_attack", "kind": "method", "class": "玩家", "name": "攻击", "visited": 12, "hasBreakpoint": True, "clusterId": "c_player"},
        {"id": "m_player_finish", "kind": "method", "class": "玩家", "name": "收尾", "visited": 2, "hasBreakpoint": False, "clusterId": "c_player"},
        {"id": "m_enemy_init", "kind": "method", "class": "敌人", "name": "初始化", "visited": 1, "hasBreakpoint": False, "clusterId": "c_enemy"},
        {"id": "m_enemy_hurt", "kind": "method", "class": "敌人", "name": "受伤", "visited": 7, "hasBreakpoint": False, "clusterId": "c_enemy"}
    ],
    "edges": [
        {"from": "g_init", "to": "m_player_start", "calls": 3},
        {"from": "m_player_start", "to": "m_player_attack", "calls": 5},
        {"from": "m_player_attack", "to": "m_enemy_hurt", "calls": 4},
        {"from": "m_player_attack", "to": "m_player_finish", "calls": 2}
    ],
    "clusters": [
        {"id": "c_player", "title": "玩家", "collapsed": False},
        {"id": "c_enemy", "title": "敌人", "collapsed": False}
    ]
}

WELCOME_HTML = b"""
<!doctype html>
<html lang=zh>
<meta charset=utf-8>
<title>文达调试 - 蓝图原型</title>
<style>
  body { font-family: system-ui, -apple-system, Segoe UI, Roboto, sans-serif; margin: 32px; }
  code { background: #f5f5f5; padding: 2px 6px; border-radius: 4px; }
</style>
<h1>文达运行时蓝图调试 - 最小原型</h1>
<p>服务已启动。你可以调用 <code>/graph/runtime</code> 获取静态蓝图数据。</p>
<p>下一步：我会在 <code>tools/debugger/web/</code> 补充前端渲染页面，并接入该接口。</p>
"""

class BlueprintHandler(SimpleHTTPRequestHandler):
    token: str | None = None

    def _auth_ok(self):
        if not self.token:
            return True
        # 简单 token 通过请求头 X-Debug-Token 传递
        t = self.headers.get('X-Debug-Token')
        return t == self.token

    def _send_json(self, obj, status=200):
        data = json.dumps(obj, ensure_ascii=False).encode('utf-8')
        self.send_response(status)
        self.send_header('Content-Type', 'application/json; charset=utf-8')
        self.send_header('Content-Length', str(len(data)))
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        self.wfile.write(data)

    def _send_html(self, data: bytes, status=200):
        self.send_response(status)
        self.send_header('Content-Type', 'text/html; charset=utf-8')
        self.send_header('Content-Length', str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def do_GET(self):
        url = urlparse(self.path)
        if url.path == '/graph/runtime':
            if not self._auth_ok():
                self._send_json({"ok": False, "error": "Unauthorized"}, status=401)
                return
            self._send_json(RUNTIME_GRAPH)
            return
        # 欢迎页
        if url.path == '/' or url.path == '/index.html':
            self._send_html(WELCOME_HTML)
            return
        # 兜底：按静态文件处理（未来用于前端资源）
        return super().do_GET()


def main():
    ap = argparse.ArgumentParser(description='Wenda Debug Blueprint Server (prototype)')
    ap.add_argument('--port', type=int, default=9230)
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--token', default=None, help='可选：简单鉴权 token')
    args = ap.parse_args()

    BlueprintHandler.token = args.token

    with HTTPServer((args.host, args.port), BlueprintHandler) as httpd:
        print(f"[blueprint] 服务已启动：http://{args.host}:{args.port}  (token={'ON' if args.token else 'OFF'})")
        print("[blueprint] /graph/runtime 提供静态蓝图数据（M1 原型）")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n[blueprint] 停止服务")

if __name__ == '__main__':
    main()
