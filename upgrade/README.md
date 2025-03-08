# Upgrade Module

This YAMC module performs a system package upgrade on Debian-based systems like Ubuntu.

## Purpose

Keep your system up to date by upgrading all installed packages to their latest versions. This module provides two upgrade modes:

1. **Regular upgrade** (default): Safe upgrade that won't remove any packages
2. **Distribution upgrade**: Full upgrade that can add or remove packages as needed

## Usage

```bash
# Perform a regular safe upgrade:
yamc -h hostname -u root upgrade

# Perform a full distribution upgrade:
yamc -h hostname -u root upgrade dist-upgrade
```

## What It Does

1. Updates the package lists (`apt-get update`)
2. Checks if any packages need upgrading
3. Performs either:
   - Regular upgrade (`apt-get upgrade`)
   - Full distribution upgrade (`apt-get dist-upgrade`)
4. Cleans up by removing unused packages (`apt-get autoremove` and `apt-get autoclean`)
5. Notifies if a system reboot is required

## When to Use Distribution Upgrade

Use the `dist-upgrade` option when:
- Upgrading the kernel
- Upgrading system components with changing dependencies
- Performing release upgrades
- You need to resolve complex dependency issues

## Requirements

- Debian-based system (Ubuntu, Debian, Linux Mint, etc.)
- Root access (`-u root`)
- Internet connectivity

## Notes

- A system reboot may be required after certain updates (especially kernel updates)
- The regular upgrade mode is safer for production systems
- The module verifies the system is Debian-based before proceeding
- If no packages need upgrading, the module will report this and exit cleanly