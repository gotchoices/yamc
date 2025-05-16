# YAMC Certbot Module

This module installs and configures Certbot for managing SSL/TLS certificates using Let's Encrypt.

## Overview

The certbot module:
- Installs the Certbot package
- Obtains SSL/TLS certificates from Let's Encrypt
- Sets up automatic certificate renewal
- Provides utilities for certificate management

> **Important**: This module requires root privileges on the remote host. Use `-u root` or a user with sudo privileges.

## Prerequisites

Depending on the authentication method you choose:

### For Standalone Mode
- No prerequisites (works out of the box)
- Port 80 must be accessible from the internet

### For Webroot Mode
- **A web server must be installed and configured before running this module**
- You can use modules like `nginx` or manually install a web server
- The web server must be properly configured to serve files from the webroot directory
- The webroot directory must exist and be accessible to the web server

## Authentication Methods

The module supports two authentication methods:

1. **Standalone Mode**:
   - No web server required (certbot runs its own temporary server)
   - Good for initial setup or servers without a web server
   - Requires port 80 to be available (will temporarily stop any running web server)

2. **Webroot Mode**:
   - Requires a running web server
   - Requires specifying the web root directory
   - Good for servers with production web services
   - Web server must be configured to serve the `/.well-known/acme-challenge/` directory

## Usage

> **Note**: For webroot mode, you must install and configure a web server before running this module. Use standalone mode if you don't have a web server installed yet.

### Basic Installation

```bash
# Using standalone mode (no web server required)
yamc -h your.server.com -u root -e standalone=true certbot

# Using webroot mode (requires running web server)
yamc -h your.server.com -u root -e webroot=/var/www/html certbot
```

### Subfunctions

#### Renew Certificates

```bash
# Force renewal of a specific certificate
yamc -h your.server.com -u root -e domain=example.com certbot renew

# Force renewal of all certificates
yamc -h your.server.com -u root certbot renew
```

#### Check Certificate Status

```bash
# Check status of a specific certificate
yamc -h your.server.com -u root -e domain=example.com certbot status

# List all certificates and their status
yamc -h your.server.com -u root certbot status
```

#### Remove Certbot

```bash
# Completely remove certbot, certificates, and related files
yamc -h your.server.com -u root certbot remove
```

## Parameters

| Parameter    | Description                                       | Default          | Required |
|--------------|---------------------------------------------------|------------------|----------|
| domain       | Domain name for the certificate                   | Target hostname  | No       |
| email        | Contact email for Let's Encrypt notifications     | admin@{domain}   | No       |
| standalone   | Use standalone mode instead of webroot            | false            | No       |
| webroot      | Web root directory for HTTP-01 challenge          | -                | Yes (if not standalone) |
| renewal_hook | Custom command to run after certificate renewal   | -                | No       |

## Examples

### Basic Certificate Configuration

```bash
# Create certificate for kjeib.com using standalone mode
export YAMC_HOST=kjeib.com
export YAMC_USER=root
yamc -e standalone=true certbot
```

### Certificate with Webroot

```bash
# Create certificate using webroot mode with nginx
yamc -h your.server.com -u root -e domain=kjeib.com -e webroot=/var/www/html certbot
```

### Using a Custom Post-Renewal Hook

```bash
yamc -h your.server.com -u root -e domain=kjeib.com -e renewal_hook="systemctl restart nginx" -e webroot=/var/www/html certbot
```

### Using Standalone Mode

```bash
# Use standalone mode (temporarily stops any web server on port 80)
yamc -h your.server.com -u root -e domain=kjeib.com -e standalone=true certbot
```

## Verification Process

When using webroot mode, the module performs several checks:

1. Verifies that a web server (nginx, apache2, httpd, or lighttpd) is running
2. Creates the required `.well-known/acme-challenge/` directory structure
3. Tests that the challenge directory is accessible via HTTP
4. Reports any potential issues before attempting certificate creation

## Notes

- Certificates are stored in `/etc/letsencrypt/live/{domain}/`
- Automatic renewal is configured via a cron job at 3:00 AM daily
- Custom post-renewal hooks can be configured using the `renewal_hook` parameter
- By default, the target hostname is used as the domain for the certificate
- In webroot mode, ensure your web server is correctly configured to serve files from the `.well-known/acme-challenge/` directory

## Troubleshooting

- Ensure port 80 is open and accessible from the internet
- Check that the domain resolves to the correct IP address
- When using webroot mode, verify your web server configuration allows access to `.well-known/acme-challenge/`
- Check certbot logs in `/var/log/letsencrypt/`
- Run `yamc -h hostname -u root certbot status` to check certificate status
- If webroot mode fails, try standalone mode with `-e standalone=true`

## Recommended Workflow

1. **Initial Server Setup**:
   - First, set up the server with basic configuration
   - If you plan to run a web server, install and configure it first

2. **Certificate Creation**:
   - If no web server is configured yet: Use standalone mode
   - If web server is already configured: Use webroot mode with the proper path

3. **For Production Use**:
   - Ensure your web server configuration properly serves the `.well-known/acme-challenge/` directory
   - Test renewal with `yamc -h hostname -u root certbot renew`
   - Set up any renewal hooks needed for your web services

This module will notify you if prerequisites are not met and provide guidance on how to proceed. 