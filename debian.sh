#!/bin/bash
APP=paintatron
VERSION=1.00
ARCH_TYPE=`uname -m`

if [ $ARCH_TYPE == "x86_64" ]; then
  ARCH_TYPE="amd64"
fi

mkdir -m 755 build
qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++ -o build/Makefile src/${APP}.pro

# Create a source archive
make clean
make source

# Build the package
fakeroot dpkg-buildpackage -F

gpg -ba ../${APP}_${VERSION}-1_${ARCH_TYPE}.deb
gpg -ba ../${APP}_${VERSION}.orig.tar.gz
