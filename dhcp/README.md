# dhcp - ISC DHCP Server Module

This module installs and configures ISC DHCP Server on a remote Ubuntu system.
Automatically detects and deploys files referenced by `include` statements.

## Prerequisites

- Ubuntu/Debian-based system
- Root access (run with `-u root`)
- Resources directory with your DHCP configuration

## Resources Structure

```
yamc.local/dhcp/
├── cluster.conf         # (Optional) List of servers for cluster deployment
├── dhcpd.conf           # Main DHCP configuration
└── hosts.conf           # (Optional) Host declarations via include
```

## Auto-Include Detection

The module automatically detects `include` statements in your configuration
and deploys the referenced files alongside the main config.

When deploying, the module scans dhcpd.conf for lines like:

```conf
include "/etc/dhcp/dhcpd-hosts.conf";
```

For each include, it looks for a matching local file:
1. Exact basename: `dhcpd-hosts.conf`
2. Without `dhcpd-` prefix: `hosts.conf`

If found, the file is deployed. If not found, an error is reported.

## Usage

### Initial Setup

Install ISC DHCP Server and deploy configuration:

```bash
yamc -h dhcp-server -u root dhcp
```

### Deploy Only (No Edit)

Deploy current configuration without opening editor:

```bash
yamc -h dhcp-server -u root dhcp deploy
```

### Edit Configuration

Edit dhcpd.conf locally, then deploy:

```bash
yamc -h dhcp-server -u root dhcp edit
```

### View Configuration (Read-Only)

View your DHCP config without deploying:

```bash
yamc -h dhcp-server dhcp view
```

### Watch Logs

Watch DHCP requests in real-time:

```bash
yamc -h dhcp-server -u root dhcp watch
```

Press Ctrl+C to stop.

## Multi-Server Deployment

For deploying to multiple DHCP servers (redundancy), use the helper scripts.

### cluster.conf

Define your servers in `yamc.local/dhcp/cluster.conf`:

```bash
# DHCP Cluster Configuration
DHCP_SERVERS="server1 server2"
```

### Helper Scripts

Located in `yamc.local/bin/`:

**Edit and deploy to all servers:**
```bash
dhcp-cluster-edit
```

**Deploy only to all servers:**
```bash
dhcp-cluster-deploy
```

These scripts read the server list from cluster.conf and deploy to each.

## Configuration Example

### dhcpd.conf

```conf
authoritative;
allow bootp;

# Include host declarations from separate file
include "/etc/dhcp/dhcpd-hosts.conf";
```

### hosts.conf

```conf
option domain-name "example.org";
option domain-name-servers 192.168.2.10, 8.8.8.8;
option subnet-mask 255.255.255.0;
default-lease-time 604800;
max-lease-time 31536000;
ddns-update-style none;

subnet 192.168.2.0 netmask 255.255.255.0 {
  option routers 192.168.2.1;
}

group {
  host server1 {
    hardware ethernet aa:bb:cc:dd:ee:ff;
    fixed-address 192.168.2.10;
  }
  # ... more hosts
}
```

## Files Deployed

| Source | Destination |
|--------|-------------|
| `dhcpd.conf` | `/etc/dhcp/dhcpd.conf` |
| `hosts.conf` | `/etc/dhcp/dhcpd-hosts.conf` (if included) |

## Subcommands Reference

| Subcommand | Description |
|------------|-------------|
| *(none)* | Install and deploy configuration |
| `deploy` | Deploy without editing |
| `edit` | Edit config and deploy |
| `view` | View config (read-only, no deploy) |
| `watch` | Watch DHCP log in real-time |

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

## Multiple DHCP Servers

For redundancy, you can run identical DHCP configurations on multiple servers.
Both servers will respond to requests, and clients will use whichever responds
first. For static reservations (host declarations), both servers give the same
answer, so there's no conflict.

Note: If you use dynamic address pools (`range` statements), you should use
ISC DHCP's failover protocol to prevent duplicate address assignments.
