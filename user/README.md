# User Module

This YAMC module creates a local system user with customizable name and description.

## Purpose

This module creates a standard system user:
- Creates a system user (default: 'admin')
- Follows the principle of least privilege (no sudo access)
- Can be customized with any username and description
- Useful for various purposes including PostgreSQL peer authentication

## Usage

```bash
# Create default admin user
yamc -h hostname -u root user

# Create custom user with custom name
yamc -h hostname -u root -e username=dbadmin user

# Create user with custom name and description
yamc -h hostname -u root -e username=appuser -e description="Application Service Account" user
```

## What It Does

1. **System User Creation**:
   - Creates a new system user with the specified username (defaults to 'admin')
   - Creates a matching group if it doesn't exist
   - Creates a regular user without elevated system privileges
   - Creates the user with a disabled password for security
   - Assigns a customizable description (gecos field)

## Requirements

- Root access on the target system

## Post-Installation

After running this module:

1. You can log in as the new user:
   ```bash
   su - username
   ```

2. Set a password for the user:
   ```bash
   sudo passwd username
   ```

3. For PostgreSQL peer authentication, you can connect as:
   ```bash
   sudo -u username psql postgres
   ```

## Common Use Cases

- **Database Users**: Create users for PostgreSQL peer authentication
- **Service Accounts**: Create dedicated users for running services
- **Application Users**: Create users for specific applications
- **Functional Users**: Create users based on function rather than person

## Security Benefits

- **Least Privilege**: Users have only the permissions needed for their purpose
- **Separation of Duties**: Different functions use different accounts
- **Reduced Attack Surface**: No unnecessary privileges granted
- **Auditable Access**: Clear ownership of actions by specific user accounts

## Notes

- Users are created with disabled passwords by default
- This module is intentionally orthogonal to other modules
- For PostgreSQL peer authentication, run this module before postgresql modules
- Module is designed to be general-purpose for any user creation needs