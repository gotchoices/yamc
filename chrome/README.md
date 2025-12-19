# Chrome Module

Install Google Chrome browser from Google's official repository.

## Usage

```bash
yamc -h hostname -u root chrome
```

## What It Does

1. Checks architecture (amd64 only)
2. Adds Google's GPG signing key (modern keyring method)
3. Adds Google's apt repository
4. Installs `google-chrome-stable`
5. Installs wrapper script (if configured) for NFS environments

## Features

- **Idempotent**: Safe to run multiple times
- **Modern keyring**: Uses `/usr/share/keyrings/` (not deprecated `apt-key`)
- **Auto-updates**: Repository enables future updates via `apt upgrade`
- **NFS wrapper**: Optional wrapper for systems with NFS-mounted home directories

## Requirements

- Ubuntu/Debian amd64 system
- Root access
- Internet connectivity

## NFS Home Directory Support

If your systems mount `/home` via NFS, Chrome's default behavior causes problems:
- **Performance**: Cache reads/writes over NFS are slow
- **Lock conflicts**: Chrome uses `SingletonLock` files that conflict across machines
- **Multi-machine**: Can't run Chrome on two machines sharing the same NFS home

### Wrapper Script

Create `yamc.local/chrome/chrome` to install a wrapper that:
- Stores Chrome profiles locally in `/var/tmp/chrome-$USER/`
- Detects and reuses existing sessions on the same host
- Cleans up stale locks from crashed Chrome instances
- Provides a shorter `chrome` command

### Wrapper Usage

```bash
# Normal launch (uses local profile)
chrome

# Named profiles for isolation
chrome -p work
chrome --profile personal

# Open a URL
chrome https://example.com
```

### Tradeoff

With the wrapper, bookmarks and passwords are stored locally (lost on reboot).
Mitigate with:
- **Chrome Sync**: Sign in to sync bookmarks/passwords to Google account
- **Manual backup**: Periodically copy `Bookmarks` file from profile

## Notes

- Chrome is only available for 64-bit (amd64) systems
- Updates will come through normal `apt upgrade`

## Alternative: Chromium

If you prefer the open-source Chromium browser:

```bash
# Via snap (recommended on Ubuntu)
snap install chromium

# Or via apt (may be older version)
apt install chromium-browser
```

This module specifically installs Google Chrome, not Chromium.

