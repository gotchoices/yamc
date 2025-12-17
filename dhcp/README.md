# dhcp - ISC DHCP Server Module

This module installs and configures ISC DHCP Server on a remote Ubuntu system.
Supports both standalone and failover (two-server) configurations.
Automatically detects and deploys files referenced by `include` statements.

## Prerequisites

- Ubuntu/Debian-based system
- Root access (run with `-u root`)
- Resources directory with your DHCP configuration

## Modes of Operation

### Standalone Mode

Single DHCP server with one configuration file. Good for simple setups.
Can use `include` statements to split configuration into multiple files.

### Failover Mode

Two DHCP servers (primary + secondary) with ISC DHCP failover protocol.
Uses a shared hosts file for MAC/IP bindings and per-server failover configs.

Benefits:
- Redundancy during maintenance or failure
- Both servers answer requests (load sharing)
- Automatic failover if one server goes down

## Resources Structure

### Standalone Mode

```
yamc.local/dhcp/
├── dhcpd.conf           # Main DHCP configuration
└── hosts.conf           # (Optional) Host declarations via include
```

### Failover Mode

```
yamc.local/dhcp/
├── primary.conf         # Failover config for primary server
├── secondary.conf       # Failover config for secondary server
└── hosts.conf           # Shared host declarations (MAC/IP bindings)
```

## Auto-Include Detection

The module automatically detects `include` statements in your configuration
and deploys the referenced files alongside the main config.

### How It Works

When deploying, the module scans the main config for lines like:

```conf
include "/etc/dhcp/dhcpd-hosts.conf";
```

For each include, it looks for a matching local file:
1. Exact basename: `dhcpd-hosts.conf`
2. Without `dhcpd-` prefix: `hosts.conf`

If found, the file is deployed to the remote path. If not found, an error
is reported and deployment stops.

### Example: Standalone with Includes

**dhcpd.conf:**
```conf
authoritative;
allow bootp;

# Include shared host declarations
include "/etc/dhcp/dhcpd-hosts.conf";
```

**hosts.conf:** (deployed as `/etc/dhcp/dhcpd-hosts.conf`)
```conf
option domain-name "example.org";
subnet 192.168.2.0 netmask 255.255.255.0 { ... }
group { ... }
```

Both files are deployed automatically with:
```bash
yamc -h dhcp-server -u root dhcp
```

### File Naming Convention

| Local File | Deployed As |
|------------|-------------|
| `hosts.conf` | `/etc/dhcp/dhcpd-hosts.conf` |
| `dhcpd-hosts.conf` | `/etc/dhcp/dhcpd-hosts.conf` |
| `options.conf` | `/etc/dhcp/dhcpd-options.conf` |

The module strips the `dhcpd-` prefix when searching, so you can use
shorter local names.

## Configuration Examples

### primary.conf / secondary.conf

Small files containing failover peer configuration and include directive:

```conf
# primary.conf example
authoritative;
allow bootp;

failover peer "dhcp-failover" {
  primary;
  address 192.168.2.30;
  port 647;
  peer address 192.168.2.31;
  peer port 647;
  max-response-delay 30;
  max-unacked-updates 10;
  load balance max seconds 3;
  mclt 1800;
  split 128;
}

include "/etc/dhcp/dhcpd-hosts.conf";
```

The `secondary.conf` is identical except:
- `secondary;` instead of `primary;`
- `address` and `peer address` values swapped
- No `mclt` or `split` directives (primary only)

### hosts.conf

Contains all your host declarations, groups, and subnet definitions:

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

## Usage

### Standalone Setup

Install and deploy single-server configuration:

```bash
yamc -h dhcp-server -u root dhcp
```

### Failover Setup

Install and deploy to primary server:

```bash
yamc -h pi-dhcp1 -u root dhcp primary
```

Install and deploy to secondary server:

```bash
yamc -h pi-dhcp2 -u root dhcp secondary
```

### Deploy Only (No Edit)

Deploy current configuration without opening editor:

```bash
yamc -h pi-dhcp1 -u root dhcp deploy
yamc -h pi-dhcp2 -u root dhcp deploy
```

Useful for syncing the secondary after editing hosts on primary.

### Edit Configuration

#### Standalone Mode

Edit the complete config and deploy:

```bash
yamc -h dhcp-server -u root dhcp edit
```

#### Failover Mode

Edit the role-specific config (primary.conf or secondary.conf):

```bash
yamc -h pi-dhcp1 -u root dhcp edit primary
yamc -h pi-dhcp2 -u root dhcp edit secondary
```

Edit the shared hosts file and deploy to one server:

```bash
yamc -h pi-dhcp1 -u root dhcp edit hosts
```

### Cluster Workflow (Helper Scripts)

Helper scripts extract server IPs directly from primary.conf/secondary.conf,
making those files the single source of truth.

Edit hosts and deploy to both servers:

```bash
dhcp-cluster-edit    # Opens editor, deploys to both
```

Deploy only (after external edit):

```bash
dhcp-cluster-deploy  # Deploys to both without editing
```

These scripts are in `yamc.local/bin/` - add to your PATH or run directly.

### View Configuration (Read-Only)

View your DHCP config without deploying:

```bash
yamc -h dhcp-server dhcp view
```

Changes made in the editor are NOT deployed. Use `edit` to deploy.

### Watch Logs

Watch DHCP requests in real-time:

```bash
yamc -h dhcp-server -u root dhcp watch
```

Press Ctrl+C to stop.

## Files Deployed

### Standalone Mode

| Source | Destination |
|--------|-------------|
| `dhcpd.conf` | `/etc/dhcp/dhcpd.conf` |
| `hosts.conf` | `/etc/dhcp/dhcpd-hosts.conf` (if included) |
| *(other includes)* | *(as specified in include path)* |

### Failover Mode

| Source | Destination |
|--------|-------------|
| `primary.conf` or `secondary.conf` | `/etc/dhcp/dhcpd.conf` |
| `hosts.conf` | `/etc/dhcp/dhcpd-hosts.conf` |

## Subcommands Reference

| Subcommand | Description |
|------------|-------------|
| *(none)* | Standalone setup - install and deploy dhcpd.conf |
| `primary` | Failover setup - deploy as primary server |
| `secondary` | Failover setup - deploy as secondary server |
| `deploy` | Deploy current config without editing |
| `edit` | Edit standalone config and deploy |
| `edit primary` | Edit primary.conf and deploy |
| `edit secondary` | Edit secondary.conf and deploy |
| `edit hosts` | Edit hosts.conf and deploy |
| `view` | View config read-only (no deploy) |
| `watch` | Watch DHCP log in real-time |

## Service

- Package: `isc-dhcp-server`
- Service name: `isc-dhcp-server`
- Configuration: `/etc/dhcp/dhcpd.conf`
- Hosts file (failover): `/etc/dhcp/dhcpd-hosts.conf`

## Network Interface

On Ubuntu, you may need to specify which interface the DHCP server listens on.
Edit `/etc/default/isc-dhcp-server` and set:

```
INTERFACESv4="eth0"
```

Replace `eth0` with your actual interface name.

## Failover Protocol Notes

ISC DHCP failover is a two-node protocol:
- Exactly one primary and one secondary
- Both servers must be able to reach each other on port 647
- Ensure firewall allows UDP 647 between the two servers

For static reservations (host declarations), both servers will give identical
answers. The failover protocol primarily manages dynamic address pools to
prevent duplicate assignments.

If you only use static reservations (no `range` statements), you can
alternatively run two identical standalone servers without the failover
protocol—both will respond correctly to any request.
