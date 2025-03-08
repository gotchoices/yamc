# VirtualBox Guest Additions Module

This YAMC module installs VirtualBox Guest Additions on a virtual machine.

## Purpose

VirtualBox Guest Additions provide enhanced integration between host and guest systems, including:
- Improved video performance with automatic resolution adjustment
- Seamless mouse integration
- Shared folders
- Shared clipboard
- Time synchronization
- Better 3D support

## Recommended Usage

For the most reliable installation:

```bash
yamc -h vm-hostname -u root guestadd
```

The module will:
1. Prompt you to insert the Guest Additions CD via the VirtualBox menu
2. Wait for you to press Enter after inserting the CD
3. Automatically attempt to mount the CD if not already mounted
4. Install the Guest Additions from the CD

This interactive approach guarantees version compatibility and avoids download issues.

## Alternative Usage Methods

```bash
# Use a local ISO file
yamc -h vm-hostname -u root guestadd /path/to/VBoxGuestAdditions.iso

# Specify a specific Guest Additions version to download
yamc -h vm-hostname -u root guestadd 7.0.14
```

## What It Does

1. Prompts for insertion of the Guest Additions CD (in default mode)
2. Installs necessary dependencies (compilers, headers, DKMS)
3. Obtains the Guest Additions installer by one of these methods:
   - Mounting the Guest Additions CD that you insert (recommended method)
   - Finding an already mounted Guest Additions CD
   - Using a provided ISO file (if specified)
   - Downloading the specified version (if explicitly requested)
4. Runs the Guest Additions installer
5. Verifies the installation by checking for loaded kernel modules
6. Creates a marker file to indicate successful installation

## Requirements

- Root access on the virtual machine
- Linux-based guest OS
- Internet access (if downloading the Guest Additions)
- Kernel headers and build tools (installed automatically)

## Supported Distributions

The module has specific support for:
- Ubuntu/Debian
- Fedora/CentOS/RHEL
- openSUSE/SUSE
- Arch Linux/Manjaro

It also includes generic handling for other distributions.

## Post-Installation

After installation, a system reboot is recommended to fully activate all Guest Additions features. You can reboot with:

```bash
shutdown -r now
```

## Troubleshooting

If installation fails:

1. Ensure the virtual machine has sufficient memory (at least 512MB)
2. Make sure the kernel headers are properly installed
3. Check the VirtualBox version compatibility with your guest OS
4. If automatic CD mounting fails, try manual mounting:
   ```bash
   sudo mkdir -p /mnt/cdrom
   sudo mount /dev/cdrom /mnt/cdrom
   cd /mnt/cdrom
   sudo ./VBoxLinuxAdditions.run
   ```
5. Check for auto-mount issues in your Linux distribution
6. Try specifying the exact VirtualBox version that matches your host:
   ```bash
   yamc -h vm-hostname -u root guestadd 7.0.14
   ```

For detailed logs, check:
```bash
cat /var/log/vboxadd-setup.log
```