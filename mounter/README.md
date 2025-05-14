# NFS Mounter Module

This YAMC module installs a user-friendly utility for mounting NFS shares with desktop integration.

## Purpose

The NFS Mounter provides a simple way to mount network file systems through:

- A desktop shortcut in the applications menu
- A command-line script for terminal use
- Automatic handling of mount/unmount operations 
- User-friendly feedback during the process

## Usage

```bash
yamc -h hostname -u username -e remote=server:/share -e local=/mount/point mounter
```

### Parameters

- `-e remote`: The NFS share to mount in format server:/path
- `-e local`: The local mount point on the client system

### Example

```bash
yamc -h raspberrypi -u pi -e remote=192.168.1.100:/data -e local=/home/pi/networkdata mounter
```

## What It Does

1. Validates the remote and local parameters
2. Creates the local mount point directory if needed
3. Installs the NFS client if not already present
4. Creates a customized mount script with the specified parameters
5. Installs a desktop shortcut in the applications menu
6. Refreshes the desktop environment (if applicable)

## Requirements

- Linux-based operating system
- sudo access for mounting operations
- Desktop environment (optional, for shortcut functionality)
- Network connectivity to the NFS server

## Files Installed

- `~/mounter.sh` - The mount script with your custom parameters
- `~/.local/share/applications/mounter.desktop` - Desktop shortcut

## Post-Installation

After installation:

1. Find the "Mounter" shortcut in your applications menu
2. Click the shortcut to mount the NFS share
3. Files will be available at your specified local mount point

Alternatively, run the script directly:

```bash
~/mounter.sh
```

## Troubleshooting

If mounting fails:

1. Verify the NFS server is running and accessible
2. Check that the specified share exists on the server
3. Ensure proper network connectivity between client and server
4. Verify the client has permission to access the share

For permission issues with the mount point, ensure your user has write access to the local directory.