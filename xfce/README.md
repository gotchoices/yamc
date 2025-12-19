# xfce - XFCE Desktop Configuration Module

Configures an Ubuntu system for graphical boot with XFCE desktop environment.

## Prerequisites

- Ubuntu system with XFCE packages installed
- Run `yamc -h host -u root packages desktop` first to install packages
- Root access

## What This Module Does

1. **Installs lightdm-gtk-greeter** - Correct greeter for XFCE (not unity-greeter)
2. **Configures LightDM** - Sets `greeter-session=lightdm-gtk-greeter` and `user-session=xfce`
3. **Sets graphical.target** - System boots to GUI instead of text console
4. **Enables lightdm** - Display manager starts on boot
5. **Installs gdmflexiserver** - Helper script for screen lock (if provided in yamc.local)
6. **Configures PAM** - Ensures gnome-keyring unlocks on login

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

### "Session ubuntu is not available" / Login fails

This happens if the wrong greeter or session is configured. Fix:
```bash
# Verify xfce session exists
ls /usr/share/xsessions/

# Check/create the config
cat /etc/lightdm/lightdm.conf.d/50-xfce.conf
# Should contain:
# [Seat:*]
# greeter-session=lightdm-gtk-greeter
# user-session=xfce

# Re-run the module to fix
yamc -h host -u root xfce
```

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

### Wrong greeter (unity-greeter instead of gtk-greeter)

```bash
apt install lightdm-gtk-greeter
# Then re-run xfce module to configure it
```

### Screen lock not working

Ensure xfce4-screensaver or xscreensaver is installed:
```bash
apt install xfce4-screensaver
```

