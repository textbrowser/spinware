#!/bin/sh

export QT_AUTO_SCREEN_SCALE_FACTOR=1

# Disable https://en.wikipedia.org/wiki/MIT-SHM.

export QT_X11_NO_MITSHM=1

if [ -r ./spinware ] && [ -x ./spinware ]
then
    exec ./spinware
    exit $?
fi

if [ -r /usr/local/spinware/spinware ] &&
   [ -x /usr/local/spinware/spinware ]
then
    cd /usr/local/spinware && exec ./spinware
    exit $?
else
    exit 1
fi
