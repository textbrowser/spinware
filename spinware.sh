#!/usr/bin/env sh

export QT_AUTO_SCREEN_SCALE_FACTOR=1

# Disable https://en.wikipedia.org/wiki/MIT-SHM.

export QT_X11_NO_MITSHM=1

if [ -r ./spinware ] && [ -x ./spinware ]
then
    echo "Launching a local Spinware."
    exec ./spinware
    exit $?
fi

if [ -r /opt/spinware/spinware ] &&
   [ -x /opt/spinware/spinware ]
then
    echo "Launching an official Spinware."
    cd /opt/spinware && exec ./spinware
    exit $?
else
    echo "Cannot locate Spinware."
    exit 1
fi
