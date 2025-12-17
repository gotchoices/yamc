# bind9 - BIND DNS Server Module

This module installs and configures BIND (named) DNS server on a remote Ubuntu system.

## Prerequisites

- Ubuntu/Debian-based system
- Root access (run with `-u root`)
- Resources directory with your DNS configuration files

## Resources Structure

```
yamc.local/bind9/
├── cluster.conf         # (Optional) Server list for cluster scripts
├── named.conf           # Main BIND configuration
├── named.local          # Local zone declarations
└── zones/               # Zone files directory
    ├── example.org      # Forward zone
    └── example.org.rev  # Reverse zone
```

## Usage

### Initial Setup

Install BIND and deploy your configuration:

```bash
yamc -h dns-server -u root bind9
```

### Deploy Only (No Edit)

Deploy current configuration without opening editor:

```bash
yamc -h dns-server -u root bind9 deploy
```

### Edit Configuration

Edit DNS files locally, then deploy:

```bash
yamc -h dns-server -u root bind9 edit
```

This opens your editor with `named.conf`, `named.local`, and zone files.
After saving, changes are deployed and the service is restarted.

### View Configuration (Read-Only)

View your DNS config without deploying:

```bash
yamc -h dns-server bind9 view
```

### Watch Logs

Watch DNS queries in real-time:

```bash
yamc -h dns-server -u root bind9 watch
```

Press Ctrl+C to stop.

## Multi-Server Deployment

For deploying to multiple DNS servers (redundancy), use the helper scripts.

### cluster.conf

Define your servers in `yamc.local/bind9/cluster.conf`:

```bash
# DNS Cluster Configuration
DNS_SERVERS="server1 server2"
```

### Helper Scripts

Located in `yamc.local/bin/`:

**Edit zones and deploy to all servers:**
```bash
bind9-cluster-edit
```

**Deploy only to all servers:**
```bash
bind9-cluster-deploy
```

These scripts read the server list from cluster.conf and deploy to each.

## Files Deployed

| Source | Destination |
|--------|-------------|
| `named.conf` | `/etc/bind/named.conf` |
| `named.local` | `/etc/bind/named.local` |
| `zones/*` | `/var/lib/bind/` |

## Subcommands Reference

| Subcommand | Description |
|------------|-------------|
| *(none)* | Install and deploy configuration |
| `deploy` | Deploy without editing |
| `edit` | Edit config and deploy |
| `view` | View config (read-only, no deploy) |
| `watch` | Watch DNS log in real-time |

## Service

- Package: `bind9`
- Service name: `named` (or `bind9`)
- Configuration: `/etc/bind/`
- Zone files: `/var/lib/bind/`
- Logs: `/var/log/named/`

## Multiple DNS Servers

For redundancy, you can run identical DNS configurations on multiple servers.
Both servers will answer queries authoritatively for your zones. Clients use
whichever responds first.

For static zone files (no dynamic updates), running multiple masters with
identical configurations works well. Changes are deployed via yamc to all
servers simultaneously.

Note: For automatic zone synchronization, you could configure BIND's native
master/slave (primary/secondary) replication, but this adds complexity that's
unnecessary when deploying via yamc.
