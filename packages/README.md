# Packages Module

Install packages from curated lists.

## Usage

```bash
# Install from 'base' list (default)
yamc -h hostname -u root packages

# Install from specific list
yamc -h hostname -u root packages desktop

# Install from multiple lists
yamc -h hostname -u root packages base desktop dev
```

## Package Lists

Create lists in `yamc.local/packages/`:

```
yamc.local/packages/
├── base.list      # Essential system packages
├── desktop.list   # Desktop environment and apps
├── dev.list       # Development tools
├── media.list     # Media players and codecs
└── server.list    # Server-specific packages
```

### List Format

One package per line. Comments start with `#`:

```
# base.list - Essential packages for all systems

# Build tools
build-essential
git
curl
wget

# Editors
vim
nano

# System monitoring
htop
iotop       # Requires root to see all processes
```

Inline comments are also supported:
```
htop        # Better than top
```

## Features

- **Graceful handling**: Missing packages are skipped with a warning, not errors
- **Idempotent**: Already-installed packages are detected and skipped
- **Summary report**: Shows what was installed, skipped, or failed
- **Multiple lists**: Combine lists in one command

## Notes on Fedora → Ubuntu

Many packages have different names between Fedora and Ubuntu:

| Fedora | Ubuntu | Notes |
|--------|--------|-------|
| `vim-enhanced` | `vim` | |
| `kernel-devel` | `linux-headers-generic` | |
| `dnf-utils` | `apt-utils` | |
| `tigervnc` | `tigervnc-viewer` | |
| `xorg-x11-apps` | `x11-apps` | |

Some packages are better installed via Snap/Flatpak on Ubuntu:
- Firefox, Thunderbird, VLC, LibreOffice (often pre-installed as snaps)
- Development tools like VS Code

## Creating Your Lists

Start with the essentials and add as needed:

```bash
# Minimal base list
build-essential
git
curl
wget
vim
htop
tree
tmux
```

The module will tell you which packages weren't found, so you can
iterate on your lists.

