class BazaarWebClient {
    constructor() {
        this.apiBase = 'http://localhost:8080/api';
        this.currentTab = 'explore';
        this.apps = [];
        this.installedApps = [];
        this.updateApps = [];
        
        this.init();
    }

    init() {
        this.setupTabs();
        this.setupSearch();
        this.setupModal();
        this.setupRefresh();
        this.loadApps();
    }

    setupTabs() {
        const tabs = document.querySelectorAll('.tab');
        tabs.forEach(tab => {
            tab.addEventListener('click', () => {
                const tabName = tab.dataset.tab;
                this.switchTab(tabName);
            });
        });
    }

    switchTab(tabName) {
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
        document.querySelectorAll('.tab-content').forEach(tc => tc.classList.remove('active'));
        
        document.querySelector(`[data-tab="${tabName}"]`).classList.add('active');
        document.getElementById(tabName).classList.add('active');
        
        this.currentTab = tabName;
        
        if (tabName === 'installed' && this.installedApps.length === 0) {
            this.loadInstalledApps();
        } else if (tabName === 'updates' && this.updateApps.length === 0) {
            this.loadUpdateApps();
        }
    }

    setupSearch() {
        const searchInput = document.getElementById('search-input');
        let timeout;
        
        searchInput.addEventListener('input', (e) => {
            clearTimeout(timeout);
            timeout = setTimeout(() => {
                this.searchApps(e.target.value);
            }, 300);
        });
    }

    setupModal() {
        const modal = document.getElementById('app-detail-modal');
        const closeBtn = document.querySelector('.close');
        
        closeBtn.addEventListener('click', () => {
            modal.classList.remove('active');
        });
        
        modal.addEventListener('click', (e) => {
            if (e.target === modal) {
                modal.classList.remove('active');
            }
        });
    }

    setupRefresh() {
        document.getElementById('refresh-btn').addEventListener('click', () => {
            this.loadApps(true);
        });
    }

    async loadApps(force = false) {
        try {
            const featuredDiv = document.getElementById('featured-apps');
            featuredDiv.innerHTML = '<div class="loading">Loading applications...</div>';
            
            const response = await fetch(`${this.apiBase}/apps`);
            if (!response.ok) throw new Error('Failed to load apps');
            
            const data = await response.json();
            this.apps = data.apps || this.getMockApps();
            
            this.renderApps(this.apps, 'featured-apps');
            this.showToast('Applications loaded successfully', 'success');
        } catch (error) {
            console.error('Error loading apps:', error);
            this.apps = this.getMockApps();
            this.renderApps(this.apps, 'featured-apps');
            this.showToast('Using demo data (server not available)', 'error');
        }
    }

    async loadInstalledApps() {
        try {
            const installedDiv = document.getElementById('installed-apps');
            installedDiv.innerHTML = '<div class="loading">Loading installed applications...</div>';
            
            const response = await fetch(`${this.apiBase}/installed`);
            if (!response.ok) throw new Error('Failed to load installed apps');
            
            const data = await response.json();
            this.installedApps = data.apps || [];
            
            this.renderApps(this.installedApps, 'installed-apps', true);
        } catch (error) {
            console.error('Error loading installed apps:', error);
            this.installedApps = this.getMockInstalledApps();
            this.renderApps(this.installedApps, 'installed-apps', true);
        }
    }

    async loadUpdateApps() {
        try {
            const updatesDiv = document.getElementById('updates-apps');
            updatesDiv.innerHTML = '<div class="loading">Checking for updates...</div>';
            
            const response = await fetch(`${this.apiBase}/updates`);
            if (!response.ok) throw new Error('Failed to load updates');
            
            const data = await response.json();
            this.updateApps = data.apps || [];
            
            this.renderApps(this.updateApps, 'updates-apps', true);
        } catch (error) {
            console.error('Error loading updates:', error);
            this.updateApps = [];
            document.getElementById('updates-apps').innerHTML = 
                '<div class="loading">No updates available</div>';
        }
    }

    renderApps(apps, containerId, showRemove = false) {
        const container = document.getElementById(containerId);
        
        if (apps.length === 0) {
            container.innerHTML = '<div class="loading">No applications found</div>';
            return;
        }
        
        container.innerHTML = apps.map(app => `
            <div class="app-card" data-app-id="${app.id}">
                <div class="app-icon">${app.icon || '📦'}</div>
                <h3>${app.name}</h3>
                <p>${app.summary || 'No description available'}</p>
                <div class="app-actions">
                    <button class="btn btn-secondary" onclick="client.showAppDetail('${app.id}')">
                        Details
                    </button>
                    ${showRemove ? 
                        `<button class="btn btn-danger" onclick="client.removeApp('${app.id}')">
                            Remove
                        </button>` :
                        `<button class="btn btn-success" onclick="client.installApp('${app.id}')">
                            Install
                        </button>`
                    }
                </div>
            </div>
        `).join('');
    }

    searchApps(query) {
        if (!query.trim()) {
            this.renderApps(this.apps, 'featured-apps');
            return;
        }
        
        const filtered = this.apps.filter(app => 
            app.name.toLowerCase().includes(query.toLowerCase()) ||
            (app.summary && app.summary.toLowerCase().includes(query.toLowerCase()))
        );
        
        this.renderApps(filtered, 'featured-apps');
    }

    async showAppDetail(appId) {
        const modal = document.getElementById('app-detail-modal');
        const content = document.getElementById('app-detail-content');
        
        const app = this.apps.find(a => a.id === appId) || 
                     this.installedApps.find(a => a.id === appId);
        
        if (!app) return;
        
        const isInstalled = this.installedApps.some(a => a.id === appId);
        
        content.innerHTML = `
            <div class="app-detail-header">
                <div class="app-detail-icon">${app.icon || '📦'}</div>
                <div class="app-detail-info">
                    <h2>${app.name}</h2>
                    <div class="app-detail-meta">
                        ${app.developer || 'Unknown Developer'} • 
                        ${app.version || 'Latest'} • 
                        ${app.license || 'Unknown License'}
                    </div>
                    <div class="app-detail-actions">
                        ${isInstalled ? 
                            `<button class="btn btn-danger" onclick="client.removeApp('${app.id}')">
                                Remove
                            </button>` :
                            `<button class="btn btn-success" onclick="client.installApp('${app.id}')">
                                Install
                            </button>`
                        }
                        <button class="btn btn-secondary" onclick="document.getElementById('app-detail-modal').classList.remove('active')">
                            Close
                        </button>
                    </div>
                </div>
            </div>
            
            <div class="app-detail-section">
                <h3>Description</h3>
                <p>${app.description || app.summary || 'No description available'}</p>
            </div>
            
            ${app.screenshots ? `
                <div class="app-detail-section">
                    <h3>Screenshots</h3>
                    <div style="display: flex; gap: 10px; flex-wrap: wrap;">
                        ${app.screenshots.map(s => `
                            <img src="${s}" style="max-width: 300px; border-radius: 8px;" alt="Screenshot">
                        `).join('')}
                    </div>
                </div>
            ` : ''}
        `;
        
        modal.classList.add('active');
    }

    async installApp(appId) {
        try {
            this.showToast(`Installing ${appId}...`);
            
            const response = await fetch(`${this.apiBase}/install`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ appId })
            });
            
            if (!response.ok) throw new Error('Install failed');
            
            this.showToast(`${appId} installed successfully!`, 'success');
            this.loadInstalledApps();
        } catch (error) {
            console.error('Install error:', error);
            this.showToast('Installation simulated (demo mode)', 'success');
        }
    }

    async removeApp(appId) {
        try {
            this.showToast(`Removing ${appId}...`);
            
            const response = await fetch(`${this.apiBase}/remove`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ appId })
            });
            
            if (!response.ok) throw new Error('Remove failed');
            
            this.showToast(`${appId} removed successfully!`, 'success');
            this.loadInstalledApps();
        } catch (error) {
            console.error('Remove error:', error);
            this.showToast('Removal simulated (demo mode)', 'success');
        }
    }

    showToast(message, type = '') {
        const toast = document.getElementById('toast');
        toast.textContent = message;
        toast.className = `toast active ${type}`;
        
        setTimeout(() => {
            toast.classList.remove('active');
        }, 3000);
    }

    getMockApps() {
        return [
            {
                id: 'org.gnome.Builder',
                name: 'GNOME Builder',
                summary: 'An IDE for GNOME',
                description: 'Builder is an IDE for GNOME that is focused on bringing the power of the platform to more developers.',
                icon: '🔨',
                developer: 'GNOME Project',
                version: '45.0',
                license: 'GPL-3.0+'
            },
            {
                id: 'org.gimp.GIMP',
                name: 'GIMP',
                summary: 'Create images and edit photographs',
                description: 'GIMP is an acronym for GNU Image Manipulation Program. It is a freely distributed program for such tasks as photo retouching, image composition and image authoring.',
                icon: '🎨',
                developer: 'GIMP Team',
                version: '2.10.36',
                license: 'GPL-3.0+'
            },
            {
                id: 'org.inkscape.Inkscape',
                name: 'Inkscape',
                summary: 'Vector Graphics Editor',
                description: 'An Open Source vector graphics editor, with capabilities similar to Illustrator, CorelDraw, or Xara X, using the W3C standard Scalable Vector Graphics (SVG) file format.',
                icon: '✏️',
                developer: 'Inkscape Team',
                version: '1.3',
                license: 'GPL-2.0+'
            },
            {
                id: 'org.blender.Blender',
                name: 'Blender',
                summary: '3D Creation Suite',
                description: 'Blender is the free and open source 3D creation suite. It supports the entirety of the 3D pipeline—modeling, rigging, animation, simulation, rendering, compositing and motion tracking.',
                icon: '🎬',
                developer: 'Blender Foundation',
                version: '4.0',
                license: 'GPL-3.0+'
            },
            {
                id: 'com.spotify.Client',
                name: 'Spotify',
                summary: 'Online music streaming service',
                description: 'Access all of your favorite music with the Spotify music streaming service.',
                icon: '🎵',
                developer: 'Spotify',
                version: '1.2.31',
                license: 'Proprietary'
            },
            {
                id: 'com.discordapp.Discord',
                name: 'Discord',
                summary: 'Chat and voice communication',
                description: 'All-in-one voice and text chat for gamers that\'s free, secure, and works on both your desktop and phone.',
                icon: '💬',
                developer: 'Discord Inc.',
                version: '0.0.40',
                license: 'Proprietary'
            }
        ];
    }

    getMockInstalledApps() {
        return [
            this.getMockApps()[0],
            this.getMockApps()[1]
        ];
    }
}

const client = new BazaarWebClient();
