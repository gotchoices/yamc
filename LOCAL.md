# YAMC Local Resources

This document describes how to use external resource directories with YAMC for maintaining authoritative configuration files outside of the YAMC repository.

## Rationale

When managing services like DNS, DHCP, or databases, you often need:

1. **Authoritative configuration files** - The "source of truth" for your configs, stored safely
2. **Site-specific data** - Zone files, IP addresses, domain names that shouldn't be in a public repo
3. **Rebuild capability** - Ability to wipe a server and restore it from your authoritative configs

YAMC modules in the repo contain generic *scripts* for installing and configuring services. Local resources contain your *actual configuration data* - the specific zone files, IP ranges, and settings for your environment.

This separation means:
- Module code can be shared/version-controlled in the YAMC repo
- Your private configs stay local (or in a separate private repo)
- You can rebuild any server from scratch using your authoritative copies

## Concepts

### Resources Directory

A directory tree parallel to the YAMC modules structure, containing your configuration files:

```
yamc.local/                    # Resources directory (sibling to yamc/)
├── bind9/                     # Resources for the 'bind9' module
│   ├── named.conf             # Your authoritative named.conf
│   ├── named.local            # Local zone includes
│   └── zones/                 # Zone files
│       ├── example.org
│       └── example.org.rev
├── dhcp/                      # Resources for 'dhcp' module
│   └── dhcpd.conf
└── postgresql/                # Resources for 'postgresql' module
    └── pg_hba.conf
```

### Profiles

Profiles allow variations of the same module configuration. For example, a DNS master server and slave servers need slightly different configs:

```
yamc.local/
├── bind9/                     # Default profile (no profile specified)
│   └── ...
├── bind9.master/              # "master" profile
│   └── ...
└── bind9.slave/               # "slave" profile
    └── ...
```

Usage:
```bash
yamc -h primary-dns bind9 setup              # Uses bind9/ (default)
yamc -h primary-dns -p master bind9 setup    # Uses bind9.master/
yamc -h secondary-dns -p slave bind9 setup   # Uses bind9.slave/
```

If no profile is specified, the base module directory is used (simple default case).

Profiles are standalone - YAMC does not automatically inherit from the base. However, scripts can implement their own fallback logic by checking `RES_BASE` when a file isn't found in `RES_DIR`.

## Resolution Order

YAMC looks for resources in this order (first match wins):

1. **`-r /explicit/path`** - Command line flag (highest priority)
2. **`YAMC_RESOURCES`** - Environment variable
3. **`../yamc.local`** - Auto-detected sibling directory (relative to yamc script location)
4. **None** - Module runs without external resources


## Environment Variables

### User-Specified

| Variable | Description |
|----------|-------------|
| `YAMC_RESOURCES` | Path to resources directory |
| `YAMC_PROFILE` | Default profile name (can be overridden with `-p`) |

### Available to Scripts

These variables are set by YAMC and available in both `.loc` and remote scripts:

| Variable | Description |
|----------|-------------|
| `RES_DIR` | Path to profile-specific resources (e.g., `yamc.local/named.master/`) |
| `RES_BASE` | Path to base resources (e.g., `yamc.local/named/`) - for optional fallback |
| `res_profile` | Profile name if specified, empty otherwise |

Notes:
- If no profile is specified, `RES_DIR` and `RES_BASE` point to the same directory
- If no resources directory exists, these variables are empty
- Scripts can implement their own fallback logic using `RES_BASE`

## Workflow: Edit Pattern

The recommended pattern for maintaining configurations:

1. **Edit locally** (in the `.loc` script) - Opens your editor on authoritative files
2. **Deploy remotely** (in the main script) - Copies files into place, restarts service

This keeps editing on your local machine where your editor preferences and tools are available, while deployment happens on the target server.

### Example: bind9/edit.loc (local script)

```bash
#!/bin/bash
# Edit authoritative DNS configuration files locally

if [ -z "$RES_DIR" ]; then
  echo "ERROR: No resource directory. Use -r flag or set YAMC_RESOURCES"
  exit 1
fi

# Backup before editing
cp "$RES_DIR/named.conf" "$RES_DIR/named.conf.bak"

# Open editor on config files
$EDITOR "$RES_DIR/named.conf" "$RES_DIR/zones/"*
```

### Example: bind9/edit (remote script)

```bash
#!/bin/bash
# Deploy edited DNS configuration to the server

# Copy authoritative configs into place
cp "$RES_DIR/named.conf" /etc/named.conf
cp "$RES_DIR/zones/"* /var/named/

# Set permissions
chown named:named /etc/named.conf /var/named/*

# Restart service
systemctl restart named
```

## Command Line Reference

```
yamc -h hostname [-p profile] [-r resources_path] module [subfunction] [args...]
```

| Flag | Description |
|------|-------------|
| `-p profile` | Use named profile (e.g., `-p master` uses `module.master/`) |
| `-r path` | Explicit path to resources directory |

## Directory Structure Example

A complete setup might look like:

```
~/share/systems/
├── yamc/                      # YAMC repository (public/shared)
│   ├── yamc                   # Main script
│   ├── bind9/                 # BIND9 module scripts
│   │   ├── setup              # Initial installation
│   │   ├── setup.loc
│   │   ├── edit               # Edit and deploy configs
│   │   └── edit.loc
│   └── dhcp/
│       └── ...
│
└── yamc.local/                # Your local resources (private)
    ├── bind9/                 # Default bind9 resources
    │   ├── named.conf
    │   └── zones/
    ├── bind9.master/          # Master server profile
    │   ├── named.conf
    │   └── zones/
    └── bind9.slave/           # Slave server profile
        └── named.conf
```

## Migration from Snap

If you're migrating from snap's `site.<name>/` structure:

| Snap | YAMC |
|------|------|
| `site.mysite/named/` | `yamc.local/bind9/` |
| `site.mysite/named/named.conf` | `yamc.local/bind9/named.conf` |
| `$sitedir` variable | `$RES_DIR` variable |

The concepts map directly - snap's site-specific overlays become YAMC's local resources.

---

## Design Summary

- **Profile flag**: `-p profile` syntax
- **Profile inheritance**: No automatic inheritance; scripts can implement fallback using `RES_BASE`
- **Path resolution**: `yamc.local/` is sibling to `yamc/` directory (script location, not cwd)
- **Environment variables**: `YAMC_RESOURCES` and `YAMC_PROFILE` supported
- **Script variables**: `RES_DIR`, `RES_BASE`, `res_profile`

