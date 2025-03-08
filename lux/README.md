# Lux Module

This YAMC module configures a Linux VM running under VirtualBox on a macOS host.

## Purpose

The Lux module streamlines setup of a development VM by:
- Creating shared directories
- Setting up VirtualBox shared folder mounts
- Configuring network and hosts entries
- Disabling the firewall for development

## Usage

```bash
# Using current username:
yamc -h vm-hostname -u root lux

# Specifying a different username:
yamc -h vm-hostname -u root lux username
```

## What It Does

1. Creates `/home/username/share` directory for sharing files with the Mac host
2. Creates `/root/systems` directory for system-wide shared files
3. Updates `/etc/hosts` with entries for the Mac host
4. Configures `/etc/fstab` with VirtualBox shared folder mounts
5. Mounts VirtualBox shared folders
6. Disables firewalld for easier development

## Requirements

- Linux VM running under VirtualBox on macOS
- Root access on the VM
- VirtualBox Guest Additions installed
- Shared folders configured in VirtualBox settings

## VirtualBox Configuration

Before running this module, configure shared folders in VirtualBox:

1. In VirtualBox Manager, select your VM and go to Settings
2. Go to Shared Folders and add:
   - Name: `share`, Path: `/Users/yourusername/share`, Auto-mount: Yes
   - Name: `systems`, Path: `/Users/yourusername/systems`, Auto-mount: Yes
3. Start your VM

## Notes

- The module automatically detects the current user if not specified
- Firewall is disabled to make development easier; not recommended for production
- NFS mount entries are included in fstab as comments (alternative to VirtualBox shares)
- The module is idempotent and can be run multiple times safely