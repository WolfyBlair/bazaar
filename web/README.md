# Bazaar Web Client

A modern web interface for browsing and managing Flatpak applications.

## Features

- 🎨 Modern, responsive UI with dark mode support
- 🔍 Search applications
- 📦 Browse featured applications
- 💾 View installed applications
- ⬆️ Check for updates
- 📱 Mobile-friendly design

## Running the Web Client

### Quick Start

From the project root directory:

```bash
./run-web-preview.sh
```

This will start the web server on port 8080. Open your browser and navigate to:

```
http://localhost:8080
```

### Manual Start

You can also start the server manually:

```bash
python3 web-server.py web 8080
```

Or specify a different port:

```bash
python3 web-server.py web 3000
```

## API Endpoints

The web server provides a REST API for managing applications:

- `GET /api/apps` - List all available applications
- `GET /api/installed` - List installed applications
- `GET /api/updates` - List applications with available updates
- `POST /api/install` - Install an application
  - Body: `{"appId": "org.example.App"}`
- `POST /api/remove` - Remove an application
  - Body: `{"appId": "org.example.App"}`

## Architecture

### Frontend (HTML/CSS/JavaScript)

- **index.html** - Main application page
- **style.css** - Styling with CSS variables for theming
- **app.js** - Client-side JavaScript application

### Backend (Python)

- **web-server.py** - Simple HTTP server with API endpoints
  - Serves static files from the web directory
  - Provides REST API for application management
  - Includes sample application data for demo purposes

## Development

### Extending the API

To add new API endpoints, edit `web-server.py`:

1. Add a new handler in `handle_api_get()` or `handle_api_post()`
2. Implement the endpoint logic
3. Return JSON responses using `send_json_response()`

### Customizing the UI

The web client uses CSS variables for theming. You can customize colors by editing the `:root` section in `style.css`.

### Connecting to Real Backend

Currently, the web client uses mock data. To connect to the actual Bazaar backend:

1. Update the C web server implementation in `src/bz-web-server.c`
2. Integrate with BzApplication and BzFlatpakInstance
3. Build the C server with meson and use it instead of the Python server

## Browser Support

The web client works on all modern browsers:

- Chrome/Chromium 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## Technologies

- Pure HTML5, CSS3, and vanilla JavaScript
- No build process or dependencies required
- Responsive design with CSS Grid and Flexbox
- Dark mode support via CSS media queries
