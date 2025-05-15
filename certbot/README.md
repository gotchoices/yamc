# YAMC Certbot Module

This module installs and configures Certbot for managing SSL/TLS certificates using Let's Encrypt.

## Overview

The certbot module:
- Installs the Certbot package
- Obtains SSL/TLS certificates from Let's Encrypt
- Sets up automatic certificate renewal
- Provides utilities for certificate management

## Usage

### Basic Installation

```bash
# Install and configure certbot with domain specified as parameter
yamc -h hostname certbot example.com

# Install with domain and other parameters specified as environment variables
yamc -h hostname -e domain=example.com -e webroot=/var/www/html -e email=admin@example.com certbot
```

### Subfunctions

#### Renew Certificates

```bash
# Force renewal of a specific certificate
yamc -h hostname -e domain=example.com certbot renew

# Force renewal of all certificates
yamc -h hostname certbot renew
```

#### Check Certificate Status

```bash
# Check status of a specific certificate
yamc -h hostname -e domain=example.com certbot status

# List all certificates and their status
yamc -h hostname certbot status
```

## Parameters

| Parameter    | Description                                       | Default          | Required |
|--------------|---------------------------------------------------|------------------|----------|
| domain       | Domain name for the certificate                   | -                | Yes      |
| email        | Contact email for Let's Encrypt notifications     | admin@{domain}   | No       |
| webroot      | Web root directory for HTTP-01 challenge          | /var/www/html    | No       |
| renewal_hook | Custom command to run after certificate renewal   | -                | No       |

## Examples

### Basic Certificate Configuration

```bash
# Create certificate for kjeib.com
yamc -h your.server.com -e domain=kjeib.com certbot
```

### Certificate with Custom Webroot and Email

```bash
yamc -h your.server.com -e domain=kjeib.com -e webroot=/home/user/www -e email=admin@kjeib.com certbot
```

### Using a Custom Post-Renewal Hook

```bash
yamc -h your.server.com -e domain=kjeib.com -e renewal_hook="systemctl restart nginx" certbot
```

## Notes

- The module uses Certbot's webroot authentication method.
- Certificates are stored in `/etc/letsencrypt/live/{domain}/`.
- Automatic renewal is configured via a cron job at 3:00 AM daily.
- Custom post-renewal hooks can be configured using the `renewal_hook` parameter.
- The `webroot` directory must be accessible by the web server for HTTP-01 challenge validation.

## Troubleshooting

- Ensure your webroot is publicly accessible via HTTP
- Check that the domain resolves to the correct IP address
- Verify port 80 is open for HTTP-01 challenge validation
- Check certbot logs in `/var/log/letsencrypt/`
- Run `yamc -h hostname -e domain=example.com certbot status` to check certificate status 