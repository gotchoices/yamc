
# PostgreSQL Module

This YAMC module installs and configures PostgreSQL database server and clients.

## Purpose

This module provides tools for:
- Installing PostgreSQL server with correct repository configuration
- Installing PostgreSQL client packages only
- Editing PostgreSQL access control configuration
- Editing PostgreSQL server configuration

## Usage

```bash
# Install PostgreSQL server (default version 13)
yamc -h hostname -u root postgresql

# Install specific PostgreSQL version
yamc -h hostname -u root -e version=14 postgresql

# Install PostgreSQL client packages only
yamc -h hostname -u root postgresql client

# Edit access configuration (pg_hba.conf)
yamc -h hostname -u root postgresql access

# Edit server configuration (postgresql.conf)
yamc -h hostname -u root postgresql config

# Upgrade PostgreSQL to a newer version
yamc -h hostname -u root -e version=14 postgresql upgrade
```

## What It Does

### Main Setup (postgresql)
1. Adds the official PostgreSQL repository
2. Installs PostgreSQL server packages
3. Ensures the PostgreSQL service is enabled and running

### Client Setup (postgresql client)
1. Adds the official PostgreSQL repository
2. Installs only the client packages needed to connect to PostgreSQL servers

### Access Configuration (postgresql access)
1. Fetches the pg_hba.conf file from the remote system to your local machine
2. Opens the file in your local editor for convenient editing
3. Detects if changes were made to the file
4. Applies the changes to the remote system
5. Sets proper permissions and reloads PostgreSQL

### Server Configuration (postgresql config)
1. Fetches the postgresql.conf file from the remote system to your local machine
2. Opens the file in your local editor for convenient editing
3. Detects if changes were made to the file
4. Applies the changes to the remote system
5. Sets proper permissions and reloads PostgreSQL

### Upgrade (postgresql upgrade)
1. Checks current PostgreSQL version and validates target version
2. Installs the new PostgreSQL version alongside the existing one
3. Uses pg_upgradecluster to migrate data to the new version
4. Updates system configuration to use the new version
5. Preserves the old cluster as a backup

## Requirements

- Debian/Ubuntu-based system
- Root access
- Internet access for package installation

## Post-Installation

After installation:
- PostgreSQL server listens on localhost by default
- Default superuser is 'postgres'
- Data is stored in /var/lib/postgresql/[version]/main/
- Configuration is in /etc/postgresql/[version]/main/

## Common Configuration Changes

For development environments, you may want to:
1. Use the `postgresql access` function to edit pg_hba.conf:
   ```
   # Allow local connections without password
   local   all     postgres    trust
   local   all     all         trust
   host    all     all     127.0.0.1/32    trust
   ```

2. Use the `postgresql config` function to edit postgresql.conf:
   ```
   listen_addresses = '*'  # Listen on all interfaces
   log_min_messages = notice  # More verbose logging
   ```

## Related Resources

For PostgreSQL documentation:
- https://www.postgresql.org/docs/

For MyCHIPs-specific PostgreSQL configuration, a separate pg-mychips module will be developed.
