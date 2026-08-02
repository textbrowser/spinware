#!/usr/bin/env sh

# Alexis Megas.

export QT_AUTO_SCREEN_SCALE_FACTOR=1

# Disable https://en.wikipedia.org/wiki/MIT-SHM.

export QT_X11_NO_MITSHM=1

if [ -f ./spinware ] && [ -r ./spinware ] && [ -x ./spinware ]
then
    echo "Launching a local Spinware."
    ./spinware
    exit $?
fi

if [ -f /opt/spinware/spinware ] && \
   [ -r /opt/spinware/spinware ] && \
   [ -x /opt/spinware/spinware ]
then
    echo "Launching an official Spinware (/opt/spinware)."
    cd /opt/spinware && ./spinware
    exit $?
fi

echo "Cannot locate Spinware."
exit 1
