#!/usr/bin/env python3

from http.server import HTTPServer, BaseHTTPRequestHandler
from http.client import HTTPSConnection
from time import sleep


class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        conn = HTTPSConnection(self.headers["Host"])
        conn.request("GET", self.path)
        r1 = conn.getresponse()
        data1 = r1.read()  # This will return entire content.
        self.send_response(200)
        self.end_headers()
        self.wfile.write(data1)


print("Listening on 192.168.7.2:4433")
httpd = HTTPServer(('192.168.7.2', 4433), SimpleHTTPRequestHandler)
httpd.serve_forever()
