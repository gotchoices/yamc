#!/bin/sh

if [ ! -d /var/tmp/kback ]; then
    mkdir -m 777 /var/tmp/kback &>/dev/null
fi
