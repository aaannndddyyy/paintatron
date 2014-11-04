APP=paintatron
VERSION=1.00
CURDIR := $(shell pwd)

# Build hardening flags
CPPFLAGS:=$(shell dpkg-buildflags --get CPPFLAGS)
CFLAGS:=$(shell dpkg-buildflags --get CFLAGS)
CXXFLAGS:=$(shell dpkg-buildflags --get CXXFLAGS)
LDFLAGS:=$(shell dpkg-buildflags --get LDFLAGS)

GUI_DIR := $(CURDIR)/build
$(shell [ -d "$(GUI_DIR)" ] || mkdir -p $(GUI_DIR))
QMAKE=qmake

all:
	${QMAKE} ${CURDIR}/src/${APP}.pro -o ${GUI_DIR}/Makefile
	$(MAKE) -C $(GUI_DIR)

debug:
	${QMAKE} ${CURDIR}/src/${APP}.pro -o ${GUI_DIR}/Makefile
	$(MAKE) -C $(GUI_DIR)/ debug

source:
	tar -cvzf ../$(APP)_$(VERSION).orig.tar.gz ../$(APP)-$(VERSION) --exclude=.bzr --exclude=.git

install:
	install -m 644 man/$(APP).1.gz /usr/share/man/man1/$(APP).1.gz
	install -m 755 build/$(APP) /usr/bin/$(APP)
	mkdir -m 755 -p /usr/share/applications
	mkdir -m 755 -p /usr/share/applications/$(APP)
	mkdir -m 755 -p /usr/share/pixmaps
	mkdir -m 755 -p /usr/share/icons
	mkdir -m 755 -p /usr/share/icons/hicolor
	mkdir -m 755 -p /usr/share/icons/hicolor/scalable
	mkdir -m 755 -p /usr/share/icons/hicolor/scalable/apps
	mkdir -m 755 -p /usr/share/icons/hicolor/24x24
	mkdir -m 755 -p /usr/share/icons/hicolor/24x24/apps
	install -m 644 desktop/$(APP).desktop /usr/share/applications/$(APP)/$(APP).desktop
	install -m 644 desktop/$(APP)_logo.png /usr/share/icons/hicolor/24x24/apps/$(APP).png
	install -m 644 desktop/$(APP)_logo.svg /usr/share/icons/hicolor/scalable/apps/$(APP).svg
	install -m 644 desktop/$(APP)_logo.svg /usr/share/pixmaps/$(APP).svg

clean:
	rm -f build/$(APP)
	rm -rf debian/$(APP)
	rm -f ../$(APP)_*.deb ../$(APP)_*.asc ../$(APP)_*.dsc ../$(APP)*.changes
	rm -f *.png
	rm -f *.sh~
	rm -f *.deb \#* \.#*
	rm -f Makefile~
	$(MAKE) -C $(GUI_DIR) clean
	rm -f $(GUI_DIR)/*.o $(GUI_DIR)/*.h $(GUI_DIR)/*.cpp $(GUI_DIR)/tgui
