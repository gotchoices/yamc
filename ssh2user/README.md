# SSH Key to User Module

This YAMC module copies your SSH public key to another user's authorized_keys file, enabling passwordless SSH access to that user's account.

## Purpose

This module automates SSH key deployment to enable passwordless SSH:
- Copies your SSH public key to any user's account
- Configures proper permissions and ownership
- Verifies SSH configuration for root login when applicable
- Provides a more general solution than the original sshroot module

## Usage

```bash
# Copy SSH key to root (default)
yamc -h hostname -u someuser ssh2user

# Copy SSH key to a different user
yamc -h hostname -u someuser -e target=admin ssh2user

# Copy SSH key to a custom user
yamc -h hostname -u someuser -e target=webapp ssh2user
```

## What It Does

1. **Key Deployment**:
   - Gets your SSH public key from authorized_keys or id_rsa.pub
   - Creates the target user's .ssh directory if needed
   - Adds your key to the target user's authorized_keys file
   - Sets proper permissions and ownership on the files

2. **Root Login Verification** (when target=root):
   - Checks the SSH server configuration
   - Provides warnings and suggestions if root login is restricted

## Requirements

- SSH key already set up for the current user (done by yamc init)
- The target user must exist on the system
- Sudo access to write to the target user's home directory

## Post-Installation

After running this module:

1. You can SSH directly to the target user's account:
   ```bash
   ssh targetuser@hostname
   ```

2. No password will be required, using your SSH key for authentication

## Common Use Cases

- **Root Access**: Set up passwordless SSH to the root account (default)
- **Database Admin**: Use with the `user` module to create a database admin
- **Application Users**: Easy access to application-specific user accounts
- **Automation**: Set up key-based login for automation scripts

## Security Considerations

- Use specific target users rather than root when possible
- Regularly audit and rotate SSH keys for security
- Consider disabling password authentication in SSH config

## Notes

- This module is a generalized version of the original sshroot module
- It defaults to root for backward compatibility
- The target user must exist before running this module
- Works perfectly with the `user` module for creating and configuring users