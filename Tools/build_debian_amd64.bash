#!/usr/bin/env bash

# Alexis Megas.

if [ ! -x /usr/bin/dpkg-deb ]
then
    echo "Please install dpkg-deb."
    exit 1
fi

if [ ! -x /usr/bin/fakeroot ]
then
    echo "Please install fakeroot."
    exit 1
fi

if [ ! -r spinware.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

# Preparing ./opt/spinware.

make distclean 2>/dev/null
mkdir -p ./opt/spinware
qmake -o Makefile spinware.pro && make -j $(nproc)
cp -p ./Icons/spinware.png ./opt/spinware/.
cp -p ./spinware ./opt/spinware/.
cp -p ./spinware.sh ./opt/spinware/.

# Preparing Spinware-x.deb.

mkdir -p spinware-debian/opt
cp -pr ./DEBIAN spinware-debian/.
cp -r ./opt/spinware spinware-debian/opt/.
fakeroot dpkg-deb --build spinware-debian Spinware-2024.08.15_amd64.deb
rm -fr ./opt
rm -fr spinware-debian
make distclean
