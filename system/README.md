# system - Generic System File Deployment Module

Deploys system configuration files from `yamc.local/system/` to the remote system,
applies sed edits, and runs post-deployment hooks.

## Directory Structure

```
yamc.local/system/
├── etc/                         # Files to COPY (mirrors filesystem)
│   ├── issue
│   ├── issue.net
│   └── profile.d/
│       ├── local.sh
│       └── umask.sh
├── usr/
│   └── local/
│       └── bin/
│           └── myscript
├── __edits__/                   # Sed scripts to apply
│   └── sshd.sed
└── __hooks__/                   # Post-deploy scripts
    └── sshd.sh
```

## Usage

```bash
yamc -h hostname -u root system
```

## File Copies

Files in `yamc.local/system/` (excluding `__*__` directories) are copied to the
same path on the remote system. Directory structure is preserved.

**Permissions:**
- Files in `*/bin/*`, `*/sbin/*`, or `*.sh` → mode 755
- All other files → mode 644
- All files owned by root:root

## Sed Edits

Place `.sed` files in `yamc.local/system/__edits__/`. Each file must have a
`#@target:` header on the first line specifying the file to edit:

```bash
# sshd.sed
#@target: /etc/ssh/sshd_config

# Add AcceptEnv for EDITOR
/AcceptEnv EDITOR_OVERRIDE/d
$ a AcceptEnv EDITOR_OVERRIDE
$ a PermitUserEnvironment yes
```

The sed commands are applied to the target file. Original is backed up to `.orig`.

## Hooks

Place executable scripts in `yamc.local/system/__hooks__/`. They run after all
copies and edits are complete. Use for service restarts, etc.

```bash
# __hooks__/sshd.sh
#!/bin/bash
systemctl restart sshd
```

## Examples

### Global Shell Configuration

```
yamc.local/system/
└── etc/
    └── profile.d/
        └── local.sh     # Custom aliases, PATH, etc.
```

### Keyboard: Caps Lock → Ctrl

```
yamc.local/system/
└── etc/
    └── X11/
        └── xorg.conf.d/
            └── 90-caps-ctrl.conf
```

### SSH Customization

```
yamc.local/system/
├── __edits__/
│   └── sshd.sed         # #@target: /etc/ssh/sshd_config
└── __hooks__/
    └── sshd.sh          # systemctl restart sshd
```

## Notes

- Edits are applied after copies, so you can copy a base file and then edit it
- Hooks run in alphabetical order
- Target files for edits must already exist on the system
- Backups are only created once (won't overwrite `.orig` on re-run)

## Caution: Idempotent Sed Scripts

To ensure the module can be run multiple times safely, **always delete before appending**
in your sed scripts:

```bash
#@target: /etc/some/config

# GOOD: Delete existing lines first, then append
/MyCustomSetting/d
$ a\
MyCustomSetting=value

# BAD: Just appending will add duplicate lines on each run
$ a\
MyCustomSetting=value
```

The pattern `delete → append` ensures running the module twice produces the same result.

