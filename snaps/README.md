# snaps - Snap Package Installation Module

Installs snap packages from lists defined in `yamc.local/snaps/`.

## Why Snaps?

Some packages on Ubuntu are only available (or work better) as snaps:
- **Firefox** - Ubuntu's default is the snap version
- **Chromium** - Only available as snap on Ubuntu
- **VS Code** - Official snap from Microsoft
- **FreeCAD, QCAD** - CAD apps with complex dependencies
- **Spotify, Discord, Slack** - Desktop apps

## Usage

### Install from default list

```bash
yamc -h workstation -u root snaps
```

### Install from specific list

```bash
yamc -h workstation -u root snaps cad
```

### Install from multiple lists

```bash
yamc -h workstation -u root snaps default cad
```

## List File Format

Create lists in `yamc.local/snaps/`:

```bash
# yamc.local/snaps/default.list
firefox                    # Just the snap name
code --classic             # With installation options
spotify                    # Desktop app
```

### Snap Options

| Option | When to Use |
|--------|-------------|
| `--classic` | Apps needing full system access (VS Code, Slack) |
| `--edge` | Bleeding edge version |
| `--beta` | Beta channel |
| `--channel=X` | Specific channel |

## Example Lists

### default.list (desktop essentials)
```
firefox
thunderbird
```

### cad.list (CAD applications)
```
qcad
freecad
openscad
```

### dev.list (development tools)
```
code --classic
postman
insomnia
```

## Notes

- Snaps auto-update in the background
- Snaps are sandboxed (may have permission limitations)
- First install may take time (downloads from Snap Store)
- Some snaps require `--classic` for full filesystem access

