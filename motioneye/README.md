# MotionEye Module

This YAMC module installs MotionEye, a web-based frontend for motion detection software.

## Purpose

MotionEye provides a user-friendly web interface for managing security cameras, with features including:

- Motion detection and recording
- Live streaming and time-lapse capabilities
- Extensive camera configuration options
- User access management
- Mobile device compatibility

## Usage

```bash
yamc -h hostname -u root motioneye
```

## What It Does

1. Checks if MotionEye is already installed (provides idempotency)
2. Installs required dependencies:
   - Python 3 and development tools
   - PIP package manager
   - SSL and curl libraries
   - Compiler tools
3. Installs MotionEye via Python pip
4. Initializes and starts the MotionEye service
5. Displays instructions for accessing the web interface

## Requirements

- Root access on the target system
- Debian/Ubuntu-based Linux distribution
- Systemd for service management
- Internet access for package installation

## Post-Installation

After installation, access the MotionEye web interface at:
```
http://your-server-ip:8765
```

Default credentials:
- Username: admin
- Password: (none - blank password)

## Configuration

MotionEye's configuration is managed through the web interface. However, advanced configuration can be found in:
- `/etc/motioneye/motioneye.conf`
- `/etc/motioneye/motion.conf`

## Troubleshooting

If installation fails:

1. Check service status: `systemctl status motioneye`
2. View logs: `journalctl -u motioneye`
3. Verify Python and pip are working: `python3 -m pip --version`
4. Check network connectivity for package installation

If the service fails to start, you may need to:
```bash
systemctl stop motioneye
systemctl start motioneye
```

## Uninstallation

To remove MotionEye:
```bash
systemctl stop motioneye
systemctl disable motioneye
pip uninstall motioneye
apt-get remove --purge python3-pip libcurl4-openssl-dev gcc libssl-dev
```