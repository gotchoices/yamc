# bind9 - BIND DNS Server Module

This module installs and configures BIND (named) DNS server on a remote system.

## Prerequisites

- Ubuntu/Debian-based system
- Root access (run with `-u root`)
- Resources directory with your DNS configuration files

## Resources Structure

Your `yamc.local/bind9/` directory should contain:

```
bind9/
├── named.conf           # Main BIND configuration
├── named.local          # Local zone declarations
└── zones/               # Zone files directory
    ├── example.org      # Forward zone
    └── example.org.rev  # Reverse zone
```

## Usage

### Initial Setup

Install BIND and deploy your authoritative configuration:

```bash
yamc -h dns-server -u root bind9
```

### Edit Configuration

Edit your authoritative DNS files locally, then deploy:

```bash
yamc -h dns-server -u root bind9 edit
```

This will:
1. Open your editor with the configuration files (locally)
2. After you save and exit, deploy the updated files to the server
3. Restart the named service

### Profiles

For different DNS server roles, use profiles:

```bash
# Master DNS server
yamc -h primary-dns -u root -p master bind9

# Slave DNS server  
yamc -h secondary-dns -u root -p slave bind9
```

Create corresponding resource directories:
- `yamc.local/bind9.master/`
- `yamc.local/bind9.slave/`

## Environment Variables

The module uses these variables from your resources:

| Variable | Description |
|----------|-------------|
| `domain` | Your domain name (e.g., "example.org") |

Set these in a `settings` file in your resources directory or pass with `-e`:

```bash
yamc -h dns-server -u root -e domain=example.org bind9 edit
```

## Files Deployed (Ubuntu/Debian)

| Source | Destination |
|--------|-------------|
| `named.conf` | `/etc/bind/named.conf` |
| `named.local` | `/etc/bind/named.local` |
| `zones/*` | `/var/lib/bind/` |

## Service

- Service name: `bind9`
- Configuration directory: `/etc/bind/`
- Zone files directory: `/var/lib/bind/`
- Log directory: `/var/log/named/`

