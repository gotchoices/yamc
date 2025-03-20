# Docker Installation Module

This YAMC module installs, configures, upgrades, and removes Docker Engine on a remote Ubuntu server.

## Features

- Installs Docker Engine, Docker CLI, and Docker Compose plugin
- Supports specific version installation
- Adds users to the docker group for non-root container management
- Supports custom Docker daemon configuration
- Dedicated upgrade functionality with backup and safety features
- Complete removal functionality with data preservation options
- Idempotent installation (safe to run multiple times)
- Verifies installation success
- Handles errors gracefully with rollback capabilities

## Prerequisites

- Ubuntu target system (Debian-based systems may also work)
- Root access on the target system
- Internet access on the target system to download packages

## Usage

### Basic Installation

To install the latest version of Docker:

```bash
yamc -h hostname -u root docker
```

### Install Specific Version

To install a specific version of Docker:

```bash
yamc -h hostname -u root docker 5:24.0.7-1~ubuntu.22.04~jammy
```

Check available versions with `apt-cache madison docker-ce` on the target system.

### Add Users to Docker Group

To install Docker and add users to the docker group (allowing them to use Docker without sudo):

```bash
yamc -h hostname -u root docker latest user1 user2 user3
```

### Upgrading Docker

To upgrade an existing Docker installation to the latest version:

```bash
yamc -h hostname -u root docker upgrade
```

To upgrade to a specific version:

```bash
yamc -h hostname -u root docker upgrade 5:24.0.7-1~ubuntu.22.04~jammy
```

The upgrade process includes:
- Backing up Docker configuration
- Listing running containers (which will be stopped during upgrade)
- Upgrading Docker packages
- Starting the Docker service
- Displaying a summary of changes

**Note:** For safety, containers that were running before the upgrade are NOT automatically restarted. The script will show which containers were running, and you should manually restart them after verifying the upgrade.

### Removing Docker

To completely remove Docker and all associated data:

```bash
yamc -h hostname -u root docker remove
```

To remove Docker packages but preserve containers, images, and volumes:

```bash
yamc -h hostname -u root docker remove true
```

The removal process includes:
- Creating backups of important Docker information (container lists, image lists, etc.)
- Stopping and removing all containers (unless data preservation is requested)
- Removing all Docker images and volumes (unless data preservation is requested)
- Uninstalling Docker packages
- Cleaning up Docker directories and configuration files
- Removing Docker repository configuration

## Configuration

### Docker Daemon Configuration

For advanced configuration, you can use environment variables:

```bash
yamc -h hostname -u root -e 'DOCKER_CONFIG={"log-driver":"json-file","log-opts":{"max-size":"10m","max-file":"3"},"dns":["8.8.8.8","8.8.4.4"]}' docker
```

This sets the specified configuration in `/etc/docker/daemon.json`.

## Common Docker Daemon Configuration Options

Here are some common configuration options for production environments:

```json
{
  "log-driver": "json-file",
  "log-opts": {
    "max-size": "10m",
    "max-file": "3"
  },
  "default-address-pools": [
    {
      "base": "172.17.0.0/16",
      "size": 24
    }
  ],
  "dns": ["8.8.8.8", "8.8.4.4"],
  "storage-driver": "overlay2",
  "no-new-privileges": true,
  "live-restore": true,
  "userland-proxy": false
}
```

## Verification

After installation or upgrade, the module will display a summary showing:
- Docker version
- Docker Compose version
- Docker service status
- Docker boot status
- Users in the docker group

## Troubleshooting

If the installation, upgrade, or removal fails:
- Check network connectivity
- Verify that the provided Docker version exists
- Check system requirements
- Review logs displayed by the script
- For upgrades and removals, check the backup directory for reference information