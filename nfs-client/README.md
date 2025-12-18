# NFS Client Module

Configure NFS mounts from a file server.

## Usage

```bash
yamc -h hostname -u root nfs-client
```

## Configuration

Create `yamc.local/nfs-client/mounts.conf`:

```bash
# Format: server:/remote/path|/local/mount|options
# Options are optional, defaults to: hard,intr,nfsvers=4

# Mount home directories from 'home' server (DNS CNAME)
home:/home|/home|hard,intr
home:/home/media|/home/media|hard,intr

# Mount setup directory
sun:/root/systems/setup|/mnt/setup|hard,intr,ro
```

### Using DNS CNAMEs

Using a CNAME like `home` instead of a specific hostname makes server migration easier. When you switch from `sun` to `uranus`:

1. Update DNS: `home CNAME uranus`
2. Clients automatically use the new server

## What It Does

1. Installs `nfs-common` package
2. Starts `rpcbind` service
3. Creates local mount points if needed
4. Adds entries to `/etc/fstab`
5. Attempts to mount (graceful failure if server unavailable)

## Mount Options

Common NFS mount options:

| Option | Description |
|--------|-------------|
| `hard` | Retry indefinitely if server unreachable (recommended) |
| `soft` | Return error after retries (may cause data corruption) |
| `intr` | Allow interrupt of pending operations |
| `nfsvers=4` | Use NFSv4 (recommended) |
| `ro` | Read-only mount |
| `noatime` | Don't update access times (performance) |

Default: `hard,intr,nfsvers=4`

## Troubleshooting

```bash
# Check if mount is working
mount -t nfs4

# Check NFS server exports
showmount -e home

# Test mount manually  
mount -t nfs4 home:/home /home

# Check rpcbind
systemctl status rpcbind
```

## Notes

- Module is idempotent - safe to run multiple times
- Failed mounts don't cause errors - they'll mount at boot
- Original `/etc/fstab` backed up to `/etc/fstab.orig`

