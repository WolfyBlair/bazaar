# Bazaar Web Client Demo

## Quick Start

The web client is now running and accessible! 🎉

### Access the Web Interface

Open your browser and navigate to:

```
http://localhost:8080
```

### Test the API

You can test the REST API endpoints using curl:

```bash
# List all applications
curl http://localhost:8080/api/apps | python3 -m json.tool

# List installed applications
curl http://localhost:8080/api/installed

# Check for updates
curl http://localhost:8080/api/updates

# Install an application (simulated)
curl -X POST http://localhost:8080/api/install \
  -H "Content-Type: application/json" \
  -d '{"appId": "org.gnome.Builder"}'

# Remove an application (simulated)
curl -X POST http://localhost:8080/api/remove \
  -H "Content-Type: application/json" \
  -d '{"appId": "org.gnome.Builder"}'
```

## Features to Try

### 1. Browse Applications
- Click on the "Explore" tab to see all available applications
- Sample applications include GNOME Builder, GIMP, Inkscape, Blender, etc.

### 2. Search Functionality
- Type in the search box to filter applications
- Search works on both app names and descriptions

### 3. View App Details
- Click the "Details" button on any application card
- Modal dialog shows full app information
- View developer, version, license, and description

### 4. Dark Mode
- The interface automatically adapts to your system's dark mode preference
- Uses CSS media queries for automatic theme switching

### 5. Responsive Design
- Resize your browser window to see the responsive layout
- Works great on mobile, tablet, and desktop sizes

### 6. Simulated Operations
- Click "Install" on any app to simulate installation
- Click "Remove" on installed apps to simulate removal
- Toast notifications show operation status

## Current Status

✅ Web server running on port 8080
✅ Static file serving working
✅ REST API responding
✅ Sample data available
✅ Full UI functionality

## Stopping the Server

To stop the web server:

```bash
# Find the process
ps aux | grep web-server

# Kill it
kill <PID>

# Or use pkill
pkill -f web-server.py
```

## Next Steps

To restart the server:

```bash
./run-web-preview.sh
```

Or manually:

```bash
python3 web-server.py web 8080
```

## Architecture Overview

```
┌──────────────────┐
│  Web Browser     │
│  localhost:8080  │
└────────┬─────────┘
         │
         │ HTTP/JSON
         │
┌────────▼─────────┐
│  Python Server   │
│  web-server.py   │
├──────────────────┤
│  REST API        │
│  - /api/apps     │
│  - /api/installed│
│  - /api/updates  │
│  - /api/install  │
│  - /api/remove   │
├──────────────────┤
│  Static Files    │
│  - index.html    │
│  - style.css     │
│  - app.js        │
└──────────────────┘
```

## Sample Applications

The demo includes these sample applications:

1. **GNOME Builder** 🔨 - An IDE for GNOME
2. **GIMP** 🎨 - Create images and edit photographs
3. **Inkscape** ✏️ - Vector Graphics Editor
4. **Blender** 🎬 - 3D Creation Suite
5. **Spotify** 🎵 - Online music streaming service
6. **Discord** 💬 - Chat and voice communication
7. **VLC** 🎥 - Media player
8. **Firefox** 🦊 - Web Browser

## Troubleshooting

### Port Already in Use

If port 8080 is already in use:

```bash
# Use a different port
python3 web-server.py web 3000
```

Then access at http://localhost:3000

### Can't Access from Browser

Make sure:
1. The server is running (`ps aux | grep web-server`)
2. You're using the correct port
3. No firewall is blocking the connection

### API Returns Empty Data

This is expected! The demo uses sample/mock data. To get real data:
1. Integrate with BzFlatpakInstance backend
2. Update the C web server implementation
3. Connect to actual Flatpak repositories

## Performance

The web client is lightweight:
- HTML: ~3 KB
- CSS: ~7 KB
- JavaScript: ~8 KB
- Total page size: ~18 KB (uncompressed)

Fast loading and responsive even on slow connections!
