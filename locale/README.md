# Locale Module

This YAMC module configures the system locale on a remote host.

## Purpose

Set the system locale on a target machine, either using a specified locale name or defaulting to en_US.UTF-8.

## Usage

```bash
# Set locale to a specific locale
yamc -h hostname -u root locale fr_FR.UTF-8

# Use the default locale (en_US.UTF-8)
yamc -h hostname -u root locale
```

## What It Does

1. If a locale argument is provided, it uses that locale
2. If no argument is provided, it uses en_US.UTF-8 as the default
3. Updates the system locale configuration in /etc/default/locale
4. Enables the locale in /etc/locale.gen if needed
5. Runs locale-gen to generate the locale data if needed
6. Updates the system default locale settings

## Requirements

- Root access is required to change system locale settings
- For full functionality, the system should have locale-gen and update-locale tools

## Common Locale Names

Some commonly used locale names:

- en_US.UTF-8 (US English)
- en_GB.UTF-8 (British English)
- fr_FR.UTF-8 (French)
- de_DE.UTF-8 (German)
- es_ES.UTF-8 (Spanish)
- it_IT.UTF-8 (Italian)
- ja_JP.UTF-8 (Japanese)
- zh_CN.UTF-8 (Simplified Chinese)
- ru_RU.UTF-8 (Russian)

## Notes

- This module is idempotent and will detect if the requested locale is already set
- A system reboot may be required for all locale changes to take full effect
- If locale-gen is not available, the module will still update /etc/default/locale
- The module will automatically set UTF-8 encoding if not specified