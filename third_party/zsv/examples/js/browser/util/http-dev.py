#!/usr/bin/env python3
import http.server
import socket
import ssl
import os
import re

host = '127.0.0.1' # '0.0.0.0'
port = 8888
use_cors = True # False # True if you need CORS headers e.g. for SharedBuffer support
protocol = 'https'

print("Listening on " + protocol + '://' + str(host) + ":" + str(port))

server_address = (host, port)

class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {
        '': 'application/octet-stream',
        '.manifest': 'text/cache-manifest',
        '.html': 'text/html',
        '.png': 'image/png',
        '.jpg': 'image/jpg',
        '.svg':	'image/svg+xml',
        '.css':	'text/css',
        '.js': 'application/x-javascript',
        '.wasm': 'application/wasm',
        '.json': 'application/json',
        '.xml': 'application/xml',
    }

    def do_GET(self):
        return http.server.SimpleHTTPRequestHandler.do_GET(self)

    def do_POST(self):
        pass

    def end_headers(self):
        # Include additional response headers here. CORS for example:
        if use_cors:
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
            self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        http.server.SimpleHTTPRequestHandler.end_headers(self)

httpd = http.server.HTTPServer(server_address, CustomHTTPRequestHandler)
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
ctx.check_hostname = False
ctx.load_cert_chain(certfile='localhost.pem')  # with key inside
httpd.socket = ctx.wrap_socket(httpd.socket, server_side=True)
httpd.serve_forever()
