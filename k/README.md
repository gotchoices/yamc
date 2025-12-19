# K Editor Module

Installs the K text editor from the official GitHub repository.

## What It Does

1. **Always fetches latest**: Clones or updates from https://github.com/gotchoices/k.git main branch
2. **Always rebuilds**: Uses `make -f Makefile.k` (handles configure automatically)
3. **Always installs**: Updates to latest version even if already installed
4. **Idempotent**: Safe to run multiple times (no harm, just gets latest)
5. **Termcap**: Installs `/etc/termcap` if needed

## Usage

```bash
yamc -h hostname -u root k
```

## Requirements

- Root access
- Internet connection for cloning
- Build dependencies: git, gcc, make, ncurses-dev

## Dependencies Installed

The module automatically installs:
- `git` (for cloning)
- `gcc` (for compilation)
- `make` (for building)
- `ncurses-dev` (terminal library required for configure)

## Behavior

The module always ensures you have the latest k46 version:

- **First run**: Clone repository → build → install
- **Subsequent runs**: Update repository → rebuild → reinstall
- **Safe re-runs**: Always gets latest, no harm in running multiple times
- **Repository cached**: Uses `/var/tmp/k-build/k` to avoid full re-clone each time

## Differences from Old Version

**Removed:**
- Pre-packaged k21.tgz archive
- `/usr/local/lib/krc` configuration file
- Daily backup cron job (`kback`)

**Added:**
- Direct GitHub cloning from main branch
- Builds from k46/ subdirectory
- Streamlined installation

## Location

Installed to `/usr/local/bin/k` (via `make install`)

## Configuration

The editor uses termcap for terminal compatibility. The module installs `/etc/termcap` if it doesn't exist.
