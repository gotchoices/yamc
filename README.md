# YAMC - Yet Another Machine Configurator

YAMC is a shell-based tool for remote machine configuration and management, allowing you to execute installation scripts, configuration tasks, and maintenance operations on remote machines through SSH.

## Overview

YAMC enables you to:
- Install and configure software packages on remote machines
- Run maintenance tasks on remote machines
- Maintain a library of reusable configuration modules
- Execute local preparation tasks before remote execution
- Share local module files with the remote machine seamlessly

## Architecture

YAMC follows these key principles:
1. Initial setup is done once per target host with the `init` command
2. Each configuration task is organized as a module in its own directory
3. Modules can contain both local and remote execution scripts
4. Local directory contents are shared with the remote system via SSH and SSHFS
5. Environment variables pass data between local and remote execution phases

## Usage

```
# Initialize a host first (required once per host)
yamc init -h remote_hostname [-u username] [-v] [-t timeout]

# Run modules after initialization
yamc -h remote_hostname [-u username] [-e var=value] [-v] [-t timeout] module [subfunction] [args...]
```

- `init`: One-time initialization command that sets up SSH key authentication and installs SSHFS
- `-h remote_hostname`: Required. Specifies the target machine to configure
- `-u username`: Optional. SSH username (defaults to current user)
- `-e var=value`: Optional. Environment variables to pass to scripts (can be used multiple times)
- `-v`: Optional. Enable verbose output for debugging
- `-t timeout`: Optional. SSH connection timeout in seconds (default: 30)
- `module`: Required. The name of the module directory containing the scripts
- `subfunction`: Optional. The script to run (defaults to "setup")
- `args`: Optional. Additional arguments passed to the module scripts

### Examples

```bash
# Initialize a new machine for YAMC (required once per host)
# Must use a regular user account with sudo privileges
yamc init -h new_machine_hostname -u regular_user

# Configure DHCP server
yamc -h new_machine_hostname dhcp

# Edit DHCP configuration
yamc -h new_machine_hostname dhcp edit

# Set timezone with argument as root user
yamc -h new_server -u root timezone America/New_York
```

## How It Works

### Initialization Phase

When you run `yamc init -h hostname -u username`:

1. YAMC checks for a local SSH key, generating one if needed
2. The SSH key is installed on the remote machine using ssh-copy-id for the specified user
3. SSHFS is installed on the remote machine if not already present
4. Host-specific configuration is saved to `~/.yamc/hostname.env`

Important: The initialization must be run with the regular, unprivileged user account on the remote machine. This user must have sudo privileges to install packages. This initialization only needs to be run once per host, setting up passwordless SSH authentication and all required dependencies.

### Module Execution Phase

After initialization, when you run `yamc -h hostname module`:

1. YAMC loads the host's saved configuration from `~/.yamc/hostname.env`
2. It determines the module directory and looks for scripts to execute
3. If a `setup.loc` script exists, it's executed locally to prepare resources
4. A temporary directory is created for data exchange
5. Variables `MOD_DIR` and `MOD_TMP` are created to reference paths for both machines
6. The local module directory is shared with the remote machine using an SFTP server and SSHFS in slave mode
7. The remote script is executed on the target machine, with access to local files
8. After execution, the temporary resources are cleaned up

### SSH File Sharing Implementation

YAMC uses a named pipe and SFTP server approach to share local directories with the remote system:

```bash
reverse_sshfs_mount() {
  local localpath="$1"
  local remotehost="$2"
  local remotepath="$3"
  local fifo="/tmp/revsshfs-$$"

  mkfifo -m600 "$fifo"

  trap 'ssh "$remotehost" fusermount -u "$remotepath"; rm -f "$fifo"' EXIT INT TERM

  < "$fifo" "$SFTP_SERVER" |
    ssh "$remotehost" sshfs -o slave ":$localpath" "$remotepath" > "$fifo"
}
```

How this works:
1. Creates a named pipe (FIFO) as a communication channel
2. Sets up a trap to handle cleanup on exit
3. Runs a local SFTP server and connects its input/output to the named pipe
4. Connects via SSH to the remote machine and runs SSHFS in slave mode
5. SSHFS connects back to the local SFTP server through the SSH connection's standard I/O

Advantages of this approach:
- Uses a single SSH connection for both file sharing and commands
- More efficient than setting up separate tunnels
- Secure (uses SSH encryption)
- Real-time access to local files without separate file transfers

## Modules

Modules are organized as directories containing:

- `setup`: The main script executed on the remote machine (default)
- `setup.loc`: Optional script executed locally before SSH connection
- Any other files or subdirectories used by the scripts
- Additional subfunction scripts with corresponding `.loc` files

When a subfunction is specified, YAMC will look for and execute scripts named after that subfunction instead of "setup". For example, if you run `yamc -h host module edit`, YAMC will execute:
- `module/edit.loc` locally (if it exists)
- `module/edit` on the remote machine

Environment variables created in the `.loc` scripts will be available to the remote scripts.

### Environment Variables

These environment variables are available to all scripts:

- `MOD_DIR`: Path to the module directory (different on local vs. remote)
- `MOD_TMP`: Path to the temporary directory for file exchange
- `ssh_user`: The username used for SSH connection
- `tgt_host`: The target hostname being configured
- Any variables defined in the `.loc` script
- Any variables passed through the command line with `-e var=value`

## Module Implementation Examples

Current implemented modules:

- `test`: Test module for verifying YAMC functionality (see Testing section below)
- `timezone`: Sets the machine's timezone
- `locale`: Sets the machine's locale
- `pref`: Installs user preferences like .bash_profile and .inputrc
- `mounter`: Installs an NFS mount script and desktop shortcut

Planned modules:
- `upgrade`: Upgrade all packages
- `dhcp`: Configure the machine as a DHCP server
- `named`: Configure the machine as a DNS server
- `mailserver`: Set up a mail server

## Prerequisites

- Bash shell environment
- SSH client on the local machine
- SSH server on the remote machine
- SSHFS package on the remote machine (will be auto-installed during init)
- SFTP server on the local machine (typically included with OpenSSH)

## Target OS

Currently targeted for Ubuntu systems. Future versions may support additional distributions with different package managers.

## Implementation Notes

- The initialization process is only run once per target host
- Host configurations are cached in `~/.yamc/hostname.env`
- Error checking has been implemented for SSH connections and command execution
- The script uses temporary directories that are cleaned up after execution
- A timeout mechanism prevents hung connections
- Interactive scripts are supported through SSH's terminal allocation

## Testing

YAMC includes a test module to verify functionality. To use it:

```bash
# Initialize the host first
yamc init -h your_remote_host

# Basic functionality test
yamc -h your_remote_host test

# Test argument passing
yamc -h your_remote_host test args arg1 arg2 "argument 3"

# Test environment variable passing
yamc -h your_remote_host -e test_var1=hello -e test_var2="world" test env

# Verbose mode for debugging
yamc -h your_remote_host -v test
```

The test module verifies:
- SSH connectivity
- SSHFS file sharing
- Environment variable passing
- Command-line argument passing
- Local and remote script execution

## Automation with YAMCITY

YAMC includes a companion script called `yamcity` that allows you to automate the execution of multiple YAMC modules in sequence. This is useful for setting up complete machine configurations through a single profile file.

### How It Works

1. Create a profile file with one YAMC command per line (without the hostname)
2. Run yamcity with the hostname and profile file
3. The script executes each command in sequence, applying the hostname to all of them
4. Comprehensive logs are saved for each command and a summary is provided

### Usage

```bash
# Basic usage
./yamcity -h hostname profile_file

# With default username
./yamcity -h hostname -u username profile_file

# With verbose output
./yamcity -h hostname -v profile_file

# Continue execution after errors
./yamcity -h hostname profile_file true
```

### Profile File Format

Profile files are simple text files with one YAMC command per line:

```
# Comments start with a hash
# Each non-comment line is appended to 'yamc -h hostname'

# First, set up SSH access for root
sshroot

# Set timezone and locale as root user
-u root timezone America/Los_Angeles

# User preferences
pref

# Run tests with arguments and environment variables
-e test_var1=hello test env
```

### Logging

The yamcity script creates a timestamped log directory for each run:

- `yamc-logs/YYYYMMDD_HHMMSS/summary.log` - Overview of all commands and their status
- `yamc-logs/YYYYMMDD_HHMMSS/cmd_N.log` - Standard output for command N
- `yamc-logs/YYYYMMDD_HHMMSS/cmd_N.err` - Standard error for command N

### Example

To automate the setup of a new server:

1. Create a profile file (`server.profile`) with the desired configuration
2. Run `./yamcity -h new_server server.profile`
3. Review the logs to ensure all modules executed successfully

## Future Enhancements

- Support for multiple distribution package managers
- Configuration file for default settings
- Module templates for easier creation of new modules
- Comprehensive logging for troubleshooting
- Module validation checks before execution
- Local caching of installation states for faster execution