#!/usr/bin/env sh

# Alexis Megas.

export QT_AUTO_SCREEN_SCALE_FACTOR=1

# Disable https://en.wikipedia.org/wiki/MIT-SHM.

export QT_X11_NO_MITSHM=1

if [ -r ./spinware ] && [ -x ./spinware ]
then
    echo "Launching a local Spinware."
    ./spinware
    exit $?
fi

if [ -r /opt/spinware/spinware ] && [ -x /opt/spinware/spinware ]
then
    echo "Launching an official Spinware."
    cd /opt/spinware && ./spinware
    exit $?
else
    echo "Cannot locate Spinware."
    exit 1
fi
