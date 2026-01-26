#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import http.server
import socketserver
import webbrowser
import threading
import json
import subprocess
import os

PORT = 8080

class MyHttpRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=os.path.dirname(os.path.abspath(__file__)), **kwargs)
    
    def end_headers(self):
        self.send_cache_headers()
        super().end_headers()
    
    def send_cache_headers(self):
        self.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
        self.send_header("Pragma", "no-cache")
        self.send_header("Expires", "0")

    def do_POST(self):
        if self.path == '/simulate':
            length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(length)
            try:
                # pharse JSON data
                data = json.loads(post_data)
                
                json_filename = 'json/simulation_data.json'
                with open(json_filename, 'w', encoding='utf-8') as json_file:
                    json.dump(data, json_file, ensure_ascii=False, indent=4)

                # response client
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Content-Length', str(len(output)))
                self.end_headers()
                self.wfile.write(json.dumps({'output': output}).encode('utf-8'))
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(str(e).encode('utf-8'))
                
def start_server():
    handler = MyHttpRequestHandler
    with socketserver.TCPServer(("", PORT), handler) as httpd:
        print(f"Serving at port {PORT}")
        httpd.serve_forever()

def open_browser():
    url = f"http://localhost:{PORT}"
    print(f"Opening {url}")
    webbrowser.open(url)

if __name__ == "__main__":
    server_thread = threading.Thread(target=start_server)
    server_thread.daemon = True
    server_thread.start()

    open_browser()
    
    try:
        while True:
            pass
    except KeyboardInterrupt:
        print("Server stopped")