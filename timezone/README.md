# Timezone Module

This YAMC module sets the system timezone on a remote host.

## Purpose

Configure the system timezone on a target machine, either using a specified timezone name or defaulting to the local machine's timezone.

## Usage

```bash
# Set timezone to a specific timezone
yamc -h hostname -u root timezone America/New_York

# Use the local machine's timezone (default)
yamc -h hostname -u root timezone
```

## What It Does

1. If a timezone argument is provided, it uses that timezone
2. If no argument is provided, it uses the local machine's timezone
3. Verifies if the timezone is already set correctly (idempotent)
4. Uses timedatectl to set the system timezone
5. Verifies the change was successful

## Requirements

- The remote system must use systemd (for timedatectl)
- Root access is required to change the system timezone

## Common Timezone Names

Some commonly used timezone names:

- America/New_York
- America/Chicago
- America/Denver
- America/Los_Angeles
- Europe/London
- Europe/Paris
- Europe/Berlin
- Asia/Tokyo
- Asia/Shanghai
- Australia/Sydney
- Pacific/Auckland

For a complete list, run `timedatectl list-timezones` on a Linux system.

## Notes

- This module is idempotent and will not make changes if the timezone is already correctly set
- The module will warn but attempt to set the timezone even if it doesn't appear in the standard list
- For non-systemd systems, manual timezone configuration is required