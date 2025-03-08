# sshroot Module

This module installs the current user's SSH public key for root access on a remote machine.

## Purpose

After running `yamc init` to set up a host with the regular user, this module enables SSH access as the root user for future YAMC modules that require root privileges.

## What it does

1. Takes the regular user's SSH public key from their authorized_keys file
2. Installs it into /root/.ssh/authorized_keys
3. Sets proper permissions on the root SSH configuration files
4. Configures SSH to only allow root login with key authentication (not password)
5. Restarts the SSH service to apply changes

## Usage

```bash
# First, initialize the host (done once)
yamc -h hostname -u regular_user init

# Then, enable root SSH access
yamc -h hostname sshroot

# Now you can run modules as root
yamc -h hostname -u root some_module
```

## Security Notes

- This module enables direct root SSH access with key authentication
- Password authentication for root is explicitly disabled
- Only run this module on machines where root SSH access is acceptable
- The module uses the same SSH key that was set up during initialization