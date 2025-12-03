# Multi-Backend Package Support

Bazaar now includes experimental support for multiple package management backends in addition to Flatpak.

## Supported Backends

### 1. Flatpak (Primary)
The main backend with full functionality including:
- Browsing Flathub
- Installing, updating, and removing applications
- Add-on support
- Full metadata and AppStream integration

### 2. APT (Experimental)
Basic support for APT package management:
- Detection of APT availability on Debian-based systems
- Backend initialization
- Foundation for future APT integration

### 3. Snap (Experimental)  
Basic support for Snap packages:
- Detection of Snap availability
- Backend initialization
- Foundation for future Snap Store integration

### 4. DEB Files (Experimental)
Support for local .deb package files:
- Validation of .deb package files using dpkg-deb
- Installation through dpkg (requires pkexec for root privileges)
- Can open .deb files from command line

## Usage

### Opening .deb Files

You can open .deb files directly from the command line:

```bash
bazaar /path/to/package.deb
```

The application will validate the package and prepare it for installation.

## Architecture

Each backend implements the `BzBackend` interface which provides methods for:
- `create_notification_channel` - Create a channel for backend notifications
- `load_local_package` - Load a local package file (e.g., .deb)
- `retrieve_remote_entries` - Fetch available packages from remote sources
- `retrieve_install_ids` - Get list of installed package IDs
- `retrieve_update_ids` - Get list of packages with available updates
- `schedule_transaction` - Execute install/update/remove operations

## Implementation Details

### Backend Initialization

All backends are initialized during application startup in `init_fiber()`:
- Flatpak backend initialization is mandatory (application fails if unavailable)
- APT, Snap, and DEB backends are optional (failures logged but don't stop startup)

### Backend Availability

Backends check for tool availability:
- **APT**: Checks for `apt` command
- **Snap**: Checks for `snap` command  
- **DEB**: Checks for `dpkg` command

If a tool is not available, the backend remains inactive but doesn't cause errors.

### File Type Detection

When opening files from the command line, Bazaar checks the file extension:
- `.flatpakref` - Handled by Flatpak backend
- `.deb` - Handled by DEB backend (if available)

## Future Development

The current implementation provides a foundation for full multi-backend support. Future enhancements may include:

1. **APT Integration**:
   - Parsing APT repositories
   - Displaying available packages
   - Full install/remove functionality
   - Update management

2. **Snap Store Integration**:
   - Browsing Snap Store
   - Installing/removing snaps
   - Channel management

3. **DEB Package Management**:
   - Rich metadata extraction from .deb files
   - Dependency resolution
   - GUI for installation confirmation

4. **Unified Package View**:
   - Single interface showing packages from all backends
   - Backend-specific metadata display
   - Smart package source selection

## Limitations

Current limitations of the experimental backends:

- No package browsing for APT/Snap
- No transaction progress tracking
- No dependency resolution
- Limited error handling
- DEB installation requires manual privilege elevation

These will be addressed in future releases as the backends mature.
