# MyCHIPs PostgreSQL Module

This YAMC module configures PostgreSQL specifically for the MyCHIPs application.

## Purpose

This module prepares a PostgreSQL installation for running MyCHIPs by:
- Installing required PostgreSQL extensions (pltcl, plpython3u)
- Configuring PostgreSQL authentication for MyCHIPs users
- Creating the admin role with appropriate permissions
- Fixing common configuration issues

## Usage

```bash
# Configure PostgreSQL for MyCHIPs with default settings (dev mode)
yamc -h hostname -u root pg-mychips

# Configure for a specific PostgreSQL version
yamc -h hostname -u root -e version=14 pg-mychips

# Configure for production mode
yamc -h hostname -u root -e mode=prod pg-mychips

# Combine both options
yamc -h hostname -u root -e version=14 -e mode=prod pg-mychips
```

## Prerequisites

Before using this module, you must have:
- PostgreSQL installed using the `postgresql` module
- Root access to the target system

## What It Does

1. **Extension Installation**:
   - Installs postgresql-pltcl and postgresql-plpython3u extensions
   - Installs tcl-dev and tcllib for additional functionality
   - Creates the pltcl and plpython3u extensions in the database

2. **Access Configuration**:
   - Backs up the original pg_hba.conf file
   - Comments out default access rules
   - Configures MyCHIPs-specific access rules:
     - **Dev mode**: Trust authentication for local connections
     - **Prod mode**: MD5 authentication for specific networks

3. **User Setup**:
   - Creates the 'admin' database role with appropriate permissions

4. **Host Configuration**:
   - Fixes IPv6 localhost configuration to prevent connection issues

## Configuration Modes

### Development Mode (default)
- Local connections use peer authentication for admin
- Trust authentication for MyCHIPs database and roles
- Host connections from 127.0.0.1 use trust authentication
- Optimized for local development

### Production Mode
- Local connections use peer authentication
- Host connections require MD5 password authentication
- More restrictive access rules for security
- Limited to 192.168.0.0/16 network by default

## Post-Installation

After running this module:

1. You can connect to PostgreSQL as the admin user:
   ```bash
   sudo -u postgres psql
   ```

2. Create a MyCHIPs database:
   ```bash
   createdb -U admin mychips
   ```

3. Connect to the MyCHIPs database:
   ```bash
   psql -U admin mychips
   ```

## Notes

- This module is specifically designed for MyCHIPs and may not be suitable for general PostgreSQL usage
- The admin role is created with superuser privileges for development convenience
- For production environments, consider adding SSL configuration (not yet implemented)

## Related Modules

- `postgresql` - Base PostgreSQL installation and configuration