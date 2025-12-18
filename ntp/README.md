# NTP Module

Installs and configures chrony for NTP time synchronization.

## Usage

```bash
# Basic usage (uses pool.ntp.org defaults)
yamc -h hostname -u root ntp

# With site-specific NTP server (configure in yamc.local)
yamc -h hostname -u root ntp
```

## What It Does

1. Installs chrony (Ubuntu's default NTP implementation)
2. Backs up original configuration
3. Optionally configures a custom NTP server
4. Optionally configures host as an NTP server
5. Enables and starts the chrony service
6. Displays synchronization status

## Site Configuration

Create `yamc.local/ntp/config.conf`:

```bash
# Point to your local NTP server
NTP_SERVER=ntp.example.com

# Set to "true" if this host should serve time to others
# NTP_IS_SERVER=true
```

If no config file exists, the module uses Ubuntu's default pool.ntp.org servers.

## Verification

After running, check NTP status with:

```bash
# Show tracking info
chronyc tracking

# Show NTP sources
chronyc sources

# Show detailed source stats
chronyc sourcestats
```

## Notes

- Chrony is preferred over ntpd on modern Ubuntu systems
- The module is idempotent - safe to run multiple times
- Original config is preserved as `/etc/chrony/chrony.conf.orig`

