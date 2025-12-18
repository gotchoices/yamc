# CUPS Module

Install CUPS and configure network printers.

## Usage

```bash
yamc -h hostname -u root cups
```

## Configuration

Create `yamc.local/cups/printers.conf`:

```bash
# Format: name|uri|description|driver|options
# driver and options are optional
# Default driver is "everywhere" (driverless IPP)

# Network printers via DNS-SD (auto-discovery URI)
B|dnssd://Brother%20MFC-L2750DW%20series._ipp._tcp.local/|Basement Laser
J|dnssd://Brother%20MFC-J6920DW._ipp._tcp.local/|Color Inkjet

# Network printers via IP
M|ipp://192.168.5.220/ipp/print|Loft Laser

# Printer requiring specific driver
L|socket://L.batemans.org|Label Printer|epson-label-driver

# Printer with options
K|ipp://192.168.5.223/ipp/print|Photo Printer||Quality=Photo
```

## Finding Printer URIs

```bash
# Discover network printers via DNS-SD
avahi-browse -t _ipp._tcp

# List available printer URIs
lpinfo -v

# For HP printers specifically
hp-probe
```

## Driverless IPP (Recommended)

Modern printers support **IPP Everywhere** - CUPS queries the printer's capabilities directly, no PPD/driver needed.

The module uses `-m everywhere` by default. Only specify a driver for:
- Old printers without IPP support
- Special printers (label, receipt, etc.)
- When driverless doesn't work correctly

## What It Does

1. Installs CUPS, hplip (HP drivers)
2. Enables CUPS service
3. Configures each printer via `lpadmin`
4. Uses driverless IPP unless driver specified

## Troubleshooting

```bash
# List configured printers
lpstat -p

# Print test page
lp -d PrinterName /usr/share/cups/data/testprint

# CUPS web interface
http://hostname:631

# Check if printer responds
ping printer-ip
lpinfo -v | grep printer-ip

# CUPS error log
tail -f /var/log/cups/error_log
```

## Notes

- If no printers.conf exists, CUPS is installed but no printers configured
- Printers are removed and re-added on each run (clean configuration)
- Web admin available at http://hostname:631

