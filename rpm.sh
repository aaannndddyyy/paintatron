# Fedora Compilation and packaging script
# Tested on Fedora 18

APP=paintatron
VERSION=1.00
SOURCEDIR=.
ARCH_TYPE=`uname -m`

sudo yum groupinstall "Development Tools"
sudo yum install rpmdevtools gnuplot libpng-devel qt4 qt4-devel libgpr

if [ ${ARCH_TYPE} == "x86_64" ]
then
    LIB=lib64
else
    LIB=lib
fi

if [ $# -ge 1 -a "$1" == "--debug" ]
then
    CFLAGS=-g
else
    CFLAGS=-O2 
fi

INCLUDES="-I/usr/include"

LIBS="-L/usr/${LIB}"

LIBS2=""

mkdir -m 755 build
qmake-qt4 -spec /usr/${LIB}/qt4/mkspecs/linux-g++ -o build/Makefile src/${APP}.pro

make clean
make

rm -rf ~/rpmbuild
sudo rm -rf rpmpackage/${APP}-${VERSION}
rpmdev-setuptree
mkdir rpmpackage/${APP}-${VERSION}

DEST_SHARE=rpmpackage/${APP}-${VERSION}/usr/share

mkdir rpmpackage/${APP}-${VERSION}/usr
mkdir rpmpackage/${APP}-${VERSION}/usr/bin
mkdir ${DEST_SHARE}
mkdir ${DEST_SHARE}/man
mkdir ${DEST_SHARE}/man/man1
install -m 755 build/${APP} rpmpackage/${APP}-${VERSION}/usr/bin
install -m 644 man/${APP}.1.gz ${DEST_SHARE}/man/man1

# desktop/menu/icon settings
mkdir -m 755 -p ${DEST_SHARE}/applications
mkdir -m 755 -p ${DEST_SHARE}/applications/${APP}
mkdir -m 755 -p ${DEST_SHARE}/pixmaps
mkdir -m 755 -p ${DEST_SHARE}/icons
mkdir -m 755 -p ${DEST_SHARE}/icons/hicolor
mkdir -m 755 -p ${DEST_SHARE}/icons/hicolor/scalable
mkdir -m 755 -p ${DEST_SHARE}/icons/hicolor/scalable/apps
mkdir -m 755 -p ${DEST_SHARE}/icons/hicolor/24x24
mkdir -m 755 -p ${DEST_SHARE}/icons/hicolor/24x24/apps
install -m 644 desktop/${APP}.desktop ${DEST_SHARE}/applications/${APP}/${APP}.desktop
install -m 644 desktop/${APP}_logo.png $DEST_SHARE/icons/hicolor/24x24/apps/${APP}.png
install -m 644 desktop/${APP}_logo.svg $DEST_SHARE/icons/hicolor/scalable/apps/${APP}.svg
install -m 644 desktop/${APP}_logo.svg $DEST_SHARE/pixmaps/${APP}.svg

cd rpmpackage
mkdir ${APP}-${VERSION}/etc
mkdir ${APP}-${VERSION}/etc/${APP}
install -m 644 ${APP}.conf ${APP}-${VERSION}/etc/${APP}/
tar -zcvf ${APP}-${VERSION}.tar.gz ${APP}-${VERSION}/

rm -rf ~/rpmbuild/BUILD/${APP}-${VERSION}
rm ~/rpmbuild/SOURCES/${APP}*.*
cp ${APP}-${VERSION}.tar.gz ~/rpmbuild/SOURCES/
cp ${APP}.spec ~/rpmbuild/SPECS/

rpmbuild -ba ~/rpmbuild/SPECS/${APP}.spec

sudo rm -rf ${APP}-${VERSION}
rm ${APP}-${VERSION}.tar.gz
cp -r ~/rpmbuild/RPMS/* .
cd ..
echo ---------------------------------------------------------
echo RPM files can be found in the rpmpackage directory
echo under architecture subdirectories.
echo ---------------------------------------------------------
