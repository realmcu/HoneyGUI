import http.server
import socketserver
import webbrowser
import threading
import os

PORT = 8000

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