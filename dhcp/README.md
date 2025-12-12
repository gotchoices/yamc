# dhcp - ISC DHCP Server Module

This module installs and configures ISC DHCP Server on a remote Ubuntu system.

## Prerequisites

- Ubuntu/Debian-based system
- Root access (run with `-u root`)
- Resources directory with your DHCP configuration

## Resources Structure

Your `yamc.local/dhcp/` directory should contain:

```
dhcp/
└── dhcpd.conf           # Your authoritative DHCP configuration
```

## Usage

### Initial Setup

Install ISC DHCP Server and deploy your configuration:

```bash
yamc -h dhcp-server -u root dhcp
```

### Edit Configuration

Edit your authoritative DHCP config locally, then deploy:

```bash
yamc -h dhcp-server -u root dhcp edit
```

### Watch Logs

Watch DHCP requests in real-time:

```bash
yamc -h dhcp-server -u root dhcp watch
```

Press Ctrl+C to stop.

### Edit Configuration

This will:
1. Open your editor with the configuration file (locally)
2. After you save and exit, deploy the updated file to the server
3. Restart the DHCP service

## Files Deployed (Ubuntu/Debian)

| Source | Destination |
|--------|-------------|
| `dhcpd.conf` | `/etc/dhcp/dhcpd.conf` |

## Service

- Package: `isc-dhcp-server`
- Service name: `isc-dhcp-server`
- Configuration: `/etc/dhcp/dhcpd.conf`

## Network Interface

On Ubuntu, you may need to specify which interface the DHCP server listens on.
Edit `/etc/default/isc-dhcp-server` and set:

```
INTERFACESv4="eth0"
```

Replace `eth0` with your actual interface name.

