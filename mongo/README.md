# MongoDB Module

This YAMC module installs and configures MongoDB on a remote system using the official MongoDB repository.

## Purpose

This module provides:
- Clean installation of MongoDB from official repositories
- Version-specific installation for compatibility
- Proper service configuration and activation
- Idempotent operation (won't reinstall if correct version exists)

## Usage

```bash
# Install default MongoDB version (7.0)
yamc -h hostname -u root mongo

# Install specific MongoDB version
yamc -h hostname -u root -e version=6.0 mongo

# Upgrade to default MongoDB version (7.0)
yamc -h hostname -u root mongo upgrade

# Upgrade to specific MongoDB version
yamc -h hostname -u root -e version=7.0 mongo upgrade
```

## What It Does

### Main Setup

1. **Version Checking**:
   - Checks if MongoDB is already installed
   - Compares installed version with requested version
   - Skips installation if correct version is already present

2. **Repository Setup**:
   - Installs required dependencies
   - Adds MongoDB GPG signing key
   - Configures MongoDB apt repository
   - Detects Ubuntu version automatically

3. **Installation and Configuration**:
   - Installs MongoDB packages
   - Configures MongoDB as a system service
   - Ensures the service is enabled and running
   - Verifies the installation was successful

### Upgrade Subfunction

1. **Prerequisites Check**:
   - Verifies MongoDB is already installed
   - Checks the current version

2. **Data Protection**:
   - Stops the MongoDB service
   - Creates a backup of all existing data
   - Maintains a timestamped backup directory

3. **Repository and Package Updates**:
   - Configures the new version's repository
   - Upgrades all MongoDB packages
   - Handles service configuration

4. **Validation**:
   - Verifies the service starts correctly
   - Confirms the version upgrade was successful
   - Provides rollback information

## Requirements

- Ubuntu-based system
- Root access
- Internet access for package installation

## Post-Installation

After installation, MongoDB will be:
- Running as a service (mongod)
- Starting automatically on system boot
- Accessible on the default port (27017)

## Configuration Details

- Configuration file: `/etc/mongod.conf`
- Log directory: `/var/log/mongodb`
- Data directory: `/var/lib/mongodb`

## Common Use Cases

- **Development Databases**: Local development environments
- **Testing and Simulation**: Test environments and simulations
- **Document Storage**: Applications requiring document-based storage
- **Microservices Backend**: Backend storage for microservices

## Notes

- The module uses the official MongoDB repositories rather than Ubuntu's packages
- MongoDB runs with default security settings (no authentication required)
- For production use, it's recommended to configure proper authentication
- Supported versions include MongoDB 5.0, 6.0, 7.0
- See https://www.mongodb.com/docs/manual/ for MongoDB documentation
