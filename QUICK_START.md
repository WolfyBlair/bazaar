# Bazaar Web Client - Quick Start Guide

## 🚀 Start the Server

```bash
./run-web-preview.sh
```

## 🌐 Access the Web Interface

Open your browser: **http://localhost:8080**

## 📡 API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/apps` | GET | List all applications |
| `/api/installed` | GET | List installed apps |
| `/api/updates` | GET | List available updates |
| `/api/install` | POST | Install application |
| `/api/remove` | POST | Remove application |

## 🧪 Quick API Tests

```bash
# List apps
curl http://localhost:8080/api/apps

# Install app
curl -X POST http://localhost:8080/api/install \
  -H "Content-Type: application/json" \
  -d '{"appId": "org.gnome.Builder"}'
```

## 🛑 Stop the Server

```bash
pkill -f web-server.py
```

## 📖 More Information

- **Full Documentation**: See `WEB_CLIENT_PREVIEW.md`
- **Demo Guide**: See `DEMO.md`
- **Implementation Details**: See `WEB_CLIENT_SUMMARY.md`
- **Web Client Docs**: See `web/README.md`

## ✅ Status Check

```bash
# Check if running
ps aux | grep web-server

# Test API
curl -s http://localhost:8080/api/apps | python3 -m json.tool
```

## 🎨 Features

- ✅ Browse applications
- ✅ Search functionality
- ✅ Install/Remove apps (demo mode)
- ✅ Dark mode support
- ✅ Responsive design
- ✅ REST API

**Everything is ready to go! Just open http://localhost:8080 in your browser.**
