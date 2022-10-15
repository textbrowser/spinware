#!/usr/bin/env bash
# Alexis Megas.

if [ ! -x /usr/bin/dpkg-deb ]; then
    echo "Please install dpkg-deb."
    exit
fi

if [ ! -x /usr/bin/fakeroot ]; then
    echo "Please install fakeroot."
    exit 1
fi

if [ ! -r spinware.pro ]; then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

# Preparing ./usr/local/spinware:

make distclean 2>/dev/null
mkdir -p ./usr/local/spinware
qmake -o Makefile spinware.pro && make -j $(nproc)
cp -p ./Icons/spinware.png ./usr/local/spinware/.
cp -p ./spinware ./usr/local/spinware/.
cp -p ./spinware.sh ./usr/local/spinware/.

# Preparing Spinware-x.deb:

mkdir -p spinware-debian/usr/local
cp -pr ./DEBIAN spinware-debian/.
cp -r ./usr/local/spinware spinware-debian/usr/local/.
fakeroot dpkg-deb --build spinware-debian Spinware-2022.10.15_amd64.deb
rm -fr ./usr
rm -fr spinware-debian
make distclean
