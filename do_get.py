#!/usr/bin/env python3

from http.server import HTTPServer, BaseHTTPRequestHandler


class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'<html>foo<b>bar</b></html>')


httpd = HTTPServer(('192.168.7.2', 80), SimpleHTTPRequestHandler)
httpd.serve_forever()
