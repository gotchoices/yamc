# Node.js Module

This YAMC module installs and configures Node.js on a remote system using the 'n' version manager.

## Purpose

This module provides:
- Clean installation of Node.js using the 'n' version manager
- Version-specific installation to ensure application compatibility
- Proper NPM configuration
- Idempotent operation (won't reinstall if correct version exists)

## Usage

```bash
# Install default Node.js version (22)
yamc -h hostname -u root node

# Install specific Node.js version
yamc -h hostname -u root -e version=16 node

# Upgrade to default Node.js version (22)
yamc -h hostname -u root node upgrade

# Upgrade to specific Node.js version
yamc -h hostname -u root -e version=20 node upgrade

# Downgrade to an earlier Node.js version (use setup script, not upgrade)
yamc -h hostname -u root -e version=16 node
```

## What It Does

### Main Setup

1. **Version Checking**:
   - Checks if Node.js is already installed
   - Compares installed version with requested version
   - Skips installation if correct version is already present

2. **Clean Installation**:
   - Installs npm package manager if needed
   - Installs 'n' Node.js version manager
   - Removes any existing Node.js installation
   - Installs the requested Node.js version

3. **Verification**:
   - Verifies the installation was successful
   - Displays the installed Node.js and npm versions

### Upgrade Subfunction

1. **Prerequisites Check**:
   - Verifies Node.js is already installed
   - Checks the current version

2. **Version Management**:
   - Installs 'n' version manager if not present
   - Uses 'n' to switch to the specified version
   - Preserves globally installed packages

3. **Verification**:
   - Confirms the upgrade was successful
   - Reports the new versions of Node.js and npm

## Requirements

- Debian/Ubuntu-based system
- Root access
- Internet access for package installation

## Post-Installation

After installation, Node.js will be available system-wide, allowing you to:
- Run Node.js applications
- Install global packages using npm
- Use npx for package execution

## Common Use Cases

- **Web Application Servers**: Deploy Node.js for running web applications
- **APIs and Microservices**: Run backend services
- **Build Tools**: Use Node.js-based build tools (webpack, gulp, etc.)
- **Development Environment**: Set up development environments

## Notes

- The module uses 'n' version manager rather than NodeSource or NVM
- It ensures a clean installation by removing any existing Node.js packages
- Major version numbers (e.g., 16, 18, 20) are supported as parameters
- To downgrade Node.js to an earlier version, use the main setup script (not upgrade)
- Downgrading involves removing the current installation and installing the earlier version