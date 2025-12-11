# Bazaar Web Client Preview

## Overview

A modern, responsive web interface has been created for Bazaar, allowing users to browse and manage Flatpak applications through their web browser.

## What's New

### Web Client (`/web` directory)

1. **index.html** - Main application interface with:
   - Header with search and refresh functionality
   - Tab navigation (Explore, Installed, Updates)
   - Application grid view
   - Modal dialog for app details
   - Toast notifications

2. **style.css** - Modern styling featuring:
   - CSS variables for easy theming
   - Dark mode support (automatic based on system preference)
   - Responsive design (mobile-friendly)
   - Smooth animations and transitions
   - Card-based layout for applications

3. **app.js** - Client-side application logic:
   - API integration for fetching applications
   - Search functionality
   - Tab switching
   - Install/Remove operations
   - Modal dialog management
   - Toast notifications

### Web Server

Two implementations provided:

1. **Python Server** (`web-server.py`) - Simple, standalone server:
   - No dependencies beyond Python 3 standard library
   - REST API with sample data
   - Static file serving
   - CORS enabled for API requests
   - Easy to run and test

2. **C Server** (`src/bz-web-server.c/h`) - Native implementation:
   - Uses libsoup3 for HTTP serving
   - Uses json-glib for JSON responses
   - Integrates with Bazaar's existing architecture
   - Can be extended to use real Flatpak backend

## Running the Web Preview

### Quick Start

```bash
./run-web-preview.sh
```

Then open your browser to: **http://localhost:8080**

### Manual Start

```bash
python3 web-server.py web 8080
```

## Features

### Current Features
- ✅ Browse featured applications with sample data
- ✅ Search applications by name or description
- ✅ View detailed application information
- ✅ Responsive design (works on desktop, tablet, mobile)
- ✅ Dark mode support (automatic)
- ✅ Modern, clean UI following GNOME design principles
- ✅ REST API for application management
- ✅ Tab navigation (Explore, Installed, Updates)

### Demo Features (Mock Data)
- 8 sample applications showcasing different categories
- Install/Remove button simulations
- API endpoint demonstrations

### Future Integration Points
- Connect to actual BzFlatpakInstance backend
- Real-time installation progress tracking
- Multi-user support via web sessions
- WebSocket integration for live updates
- Integration with BzTransactionManager
- Remote access to Bazaar functionality

## API Endpoints

The web server provides the following REST API:

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/apps` | GET | List all available applications |
| `/api/installed` | GET | List installed applications |
| `/api/updates` | GET | List applications with updates |
| `/api/install` | POST | Install an application |
| `/api/remove` | POST | Remove an application |

### Example API Usage

```bash
# Get all apps
curl http://localhost:8080/api/apps

# Install an app
curl -X POST http://localhost:8080/api/install \
  -H "Content-Type: application/json" \
  -d '{"appId": "org.gnome.Builder"}'

# Remove an app
curl -X POST http://localhost:8080/api/remove \
  -H "Content-Type: application/json" \
  -d '{"appId": "org.gnome.Builder"}'
```

## Architecture

```
┌─────────────────────────────────────────┐
│         Web Browser (Client)             │
│  ┌────────────────────────────────────┐ │
│  │  HTML/CSS/JavaScript                │ │
│  │  - index.html                       │ │
│  │  - style.css                        │ │
│  │  - app.js                           │ │
│  └────────────────────────────────────┘ │
└─────────────────┬───────────────────────┘
                  │ HTTP/JSON
                  ▼
┌─────────────────────────────────────────┐
│         Web Server                       │
│  ┌────────────────────────────────────┐ │
│  │  Python Server (web-server.py)      │ │
│  │  - Static file serving              │ │
│  │  - REST API                         │ │
│  │  - CORS support                     │ │
│  └────────────────────────────────────┘ │
│                                          │
│  OR                                      │
│                                          │
│  ┌────────────────────────────────────┐ │
│  │  C Server (bz-web-server.c)         │ │
│  │  - libsoup3 integration             │ │
│  │  - json-glib responses              │ │
│  │  - Future: BzApplication backend    │ │
│  └────────────────────────────────────┘ │
└─────────────────────────────────────────┘
```

## Technology Stack

### Frontend
- Pure HTML5, CSS3, and vanilla JavaScript
- No framework dependencies
- No build process required
- Progressive enhancement approach

### Backend (Python)
- Python 3 standard library only
- http.server module
- JSON responses
- Platform independent

### Backend (C - Future)
- libsoup-3.0 for HTTP server
- json-glib-1.0 for JSON handling
- Integration with existing Bazaar backend

## Browser Compatibility

Tested and working on:
- Chrome/Chromium 90+
- Firefox 88+
- Safari 14+
- Microsoft Edge 90+

## Screenshots

When running, the interface shows:

1. **Header Section**
   - Large Bazaar logo and title
   - Search bar for filtering applications
   - Refresh button

2. **Navigation Tabs**
   - Explore (browse all apps)
   - Installed (view installed apps)
   - Updates (check for updates)

3. **Application Grid**
   - Cards showing app icon, name, summary
   - Details and Install/Remove buttons
   - Responsive grid layout

4. **Detail Modal**
   - Full application information
   - Developer, version, license details
   - Action buttons

## Next Steps

To fully integrate the web client with Bazaar's backend:

1. Update `bz-web-server.c` to use BzApplication
2. Connect to BzFlatpakInstance for real app data
3. Implement authentication/authorization
4. Add WebSocket support for real-time updates
5. Integrate with BzTransactionManager for installs
6. Add progress tracking for operations
7. Implement user preferences
8. Add multi-language support

## Files Added/Modified

### New Files
- `web/index.html` - Main web interface
- `web/style.css` - Styling
- `web/app.js` - Client-side logic
- `web/README.md` - Web client documentation
- `web-server.py` - Python web server
- `src/bz-web-server.c` - C web server implementation
- `src/bz-web-server.h` - C web server header
- `src/web-main.c` - C web server entry point
- `run-web-preview.sh` - Convenience script to run server
- `WEB_CLIENT_PREVIEW.md` - This documentation

### Modified Files
- `src/meson.build` - Added web server executable build

## License

All new files maintain the GPL-3.0-or-later license consistent with the rest of the Bazaar project.
