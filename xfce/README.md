# xfce - XFCE Desktop Configuration Module

Configures an Ubuntu system for graphical boot with XFCE desktop environment.

## Prerequisites

- Ubuntu system with XFCE packages installed
- Run `yamc -h host -u root packages desktop` first to install packages
- Root access

## What This Module Does

1. **Sets graphical.target** - System boots to GUI instead of text console
2. **Enables lightdm** - Display manager starts on boot
3. **Installs gdmflexiserver** - Helper script for screen lock (if provided in yamc.local)
4. **Configures PAM** - Ensures gnome-keyring unlocks on login

## Usage

### Standard Setup (after packages)

```bash
# First install packages
yamc -h workstation -u root packages desktop

# Then configure graphical boot
yamc -h workstation -u root xfce
```

### In yamcity Profile

```bash
# yamc.local/profiles/workstation
-u root packages desktop
-u root xfce
-u root chrome
-u root cups
```

## Optional: gdmflexiserver Helper

If you want a screen lock + user switch helper (useful for multi-user systems),
create `yamc.local/xfce/gdmflexiserver`:

```bash
#!/bin/sh
# Lock screen and show login greeter for user switching
xflock4  # XFCE's built-in lock, or use xscreensaver-command -lock
dm-tool switch-to-greeter
```

This will be installed to `/usr/local/bin/gdmflexiserver`.

## Notes

- This module does NOT start lightdm immediately (would disconnect SSH session)
- Reboot the system or run `systemctl start lightdm` manually
- XFCE session will be available in lightdm's session selector

## Troubleshooting

### Black screen after boot

Check if lightdm is running:
```bash
systemctl status lightdm
```

Check Xorg logs:
```bash
cat /var/log/Xorg.0.log | grep EE
```

### Wrong display manager

If another DM (gdm3) was installed, reconfigure:
```bash
dpkg-reconfigure lightdm
```

### Screen lock not working

Ensure xfce4-screensaver or xscreensaver is installed:
```bash
apt install xfce4-screensaver
```

