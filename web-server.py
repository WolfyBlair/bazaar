#!/usr/bin/env python3
"""
Simple web server for Bazaar web client preview
"""

import http.server
import socketserver
import json
import os
import sys
from urllib.parse import urlparse, parse_qs
from pathlib import Path

PORT = 8080
WEB_ROOT = Path(__file__).parent / "web"

SAMPLE_APPS = [
    {
        "id": "org.gnome.Builder",
        "name": "GNOME Builder",
        "summary": "An IDE for GNOME",
        "description": "Builder is an IDE for GNOME that is focused on bringing the power of the platform to more developers.",
        "icon": "🔨",
        "developer": "GNOME Project",
        "version": "45.0",
        "license": "GPL-3.0+"
    },
    {
        "id": "org.gimp.GIMP",
        "name": "GIMP",
        "summary": "Create images and edit photographs",
        "description": "GIMP is an acronym for GNU Image Manipulation Program. It is a freely distributed program for such tasks as photo retouching, image composition and image authoring.",
        "icon": "🎨",
        "developer": "GIMP Team",
        "version": "2.10.36",
        "license": "GPL-3.0+"
    },
    {
        "id": "org.inkscape.Inkscape",
        "name": "Inkscape",
        "summary": "Vector Graphics Editor",
        "description": "An Open Source vector graphics editor, with capabilities similar to Illustrator, CorelDraw, or Xara X, using the W3C standard Scalable Vector Graphics (SVG) file format.",
        "icon": "✏️",
        "developer": "Inkscape Team",
        "version": "1.3",
        "license": "GPL-2.0+"
    },
    {
        "id": "org.blender.Blender",
        "name": "Blender",
        "summary": "3D Creation Suite",
        "description": "Blender is the free and open source 3D creation suite. It supports the entirety of the 3D pipeline—modeling, rigging, animation, simulation, rendering, compositing and motion tracking.",
        "icon": "🎬",
        "developer": "Blender Foundation",
        "version": "4.0",
        "license": "GPL-3.0+"
    },
    {
        "id": "com.spotify.Client",
        "name": "Spotify",
        "summary": "Online music streaming service",
        "description": "Access all of your favorite music with the Spotify music streaming service.",
        "icon": "🎵",
        "developer": "Spotify",
        "version": "1.2.31",
        "license": "Proprietary"
    },
    {
        "id": "com.discordapp.Discord",
        "name": "Discord",
        "summary": "Chat and voice communication",
        "description": "All-in-one voice and text chat for gamers that's free, secure, and works on both your desktop and phone.",
        "icon": "💬",
        "developer": "Discord Inc.",
        "version": "0.0.40",
        "license": "Proprietary"
    },
    {
        "id": "org.videolan.VLC",
        "name": "VLC",
        "summary": "Media player",
        "description": "VLC is a free and open source cross-platform multimedia player and framework that plays most multimedia files.",
        "icon": "🎥",
        "developer": "VideoLAN",
        "version": "3.0.20",
        "license": "GPL-2.0+"
    },
    {
        "id": "org.mozilla.firefox",
        "name": "Firefox",
        "summary": "Web Browser",
        "description": "Browse the Web with the Firefox web browser.",
        "icon": "🦊",
        "developer": "Mozilla",
        "version": "121.0",
        "license": "MPL-2.0"
    }
]


class BazaarHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(WEB_ROOT), **kwargs)
    
    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        super().end_headers()
    
    def do_OPTIONS(self):
        self.send_response(200)
        self.end_headers()
    
    def do_GET(self):
        parsed_path = urlparse(self.path)
        
        if parsed_path.path.startswith('/api/'):
            self.handle_api_get(parsed_path.path)
        else:
            super().do_GET()
    
    def do_POST(self):
        parsed_path = urlparse(self.path)
        
        if parsed_path.path.startswith('/api/'):
            content_length = int(self.headers.get('Content-Length', 0))
            body = self.rfile.read(content_length)
            try:
                data = json.loads(body) if body else {}
            except:
                data = {}
            self.handle_api_post(parsed_path.path, data)
        else:
            self.send_error(404)
    
    def handle_api_get(self, path):
        if path == '/api/apps':
            self.send_json_response({"apps": SAMPLE_APPS})
        elif path == '/api/installed':
            self.send_json_response({"apps": []})
        elif path == '/api/updates':
            self.send_json_response({"apps": []})
        else:
            self.send_error(404)
    
    def handle_api_post(self, path, data):
        if path == '/api/install':
            app_id = data.get('appId', '')
            print(f"📦 Install requested: {app_id}")
            self.send_json_response({"success": True, "message": f"Installing {app_id}"})
        elif path == '/api/remove':
            app_id = data.get('appId', '')
            print(f"🗑️  Remove requested: {app_id}")
            self.send_json_response({"success": True, "message": f"Removing {app_id}"})
        else:
            self.send_error(404)
    
    def send_json_response(self, data):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())
    
    def log_message(self, format, *args):
        print(f"[{self.log_date_time_string()}] {format % args}")


def main():
    global PORT, WEB_ROOT
    
    if len(sys.argv) > 1:
        WEB_ROOT = Path(sys.argv[1])
    
    if len(sys.argv) > 2:
        PORT = int(sys.argv[2])
    
    print("🏪 Bazaar Web Preview Server")
    print("=" * 50)
    print(f"Web root: {WEB_ROOT}")
    print(f"Port: {PORT}")
    print()
    print(f"🌐 Web Interface: http://localhost:{PORT}")
    print(f"📡 API Endpoint:  http://localhost:{PORT}/api")
    print()
    print("Press Ctrl+C to stop the server")
    print("=" * 50)
    print()
    
    try:
        with socketserver.TCPServer(("", PORT), BazaarHandler) as httpd:
            httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n\n👋 Shutting down server...")
        sys.exit(0)


if __name__ == "__main__":
    main()
