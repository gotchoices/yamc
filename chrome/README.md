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

## Features

- **Idempotent**: Safe to run multiple times
- **Modern keyring**: Uses `/usr/share/keyrings/` (not deprecated `apt-key`)
- **Auto-updates**: Repository enables future updates via `apt upgrade`

## Requirements

- Ubuntu/Debian amd64 system
- Root access
- Internet connectivity

## Notes

- Chrome is only available for 64-bit (amd64) systems
- No yamc.local configuration needed - this is a fully generic module
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

