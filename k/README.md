# K Editor Module

This YAMC module installs the K editor, a lightweight text editor for Unix-like systems.

## Purpose

The K editor provides a simple yet powerful text editing experience with features including:

- Lightweight and fast operation
- Support for standard editing operations
- Configurable through the krc file
- Automatic backup functionality

## Usage

```bash
yamc -h hostname -u root k
```

## What It Does

1. Installs necessary dependencies (gcc, make)
2. Unpacks the k21.tgz source archive to a build directory in /var/tmp
3. Builds the K editor from source
4. Installs the editor system-wide
5. Sets up:
   - Termcap file in /etc if needed
   - Backup directory in /var/tmp/kback
   - Configuration file in /usr/local/lib/krc (via Makefile)
   - Daily backup via cron

## Requirements

- Root access on the target system
- Linux-based operating system
- Build tools (gcc, make) will be installed if missing

## Files

- `k21.tgz`: Source archive for the K editor
- `krc`: Configuration file for the editor
- `termcap`: Terminal capabilities database
- `setup.kback`: Script to set up backup functionality

## Post-Installation

After installation, the K editor will be available as the command `k` in the system PATH.

## Troubleshooting

If installation fails:

1. Check that gcc and make are available
2. Verify that the source unpacks correctly
3. Look for compilation errors in the build output
4. Ensure the target system has sufficient disk space

For systems where the automatic installation doesn't work, you can try manual installation:

```bash
# Extract the source
tar -xzf k21.tgz
cd k21
# Build and install
make
make install  # or: cp k /usr/local/bin/
```