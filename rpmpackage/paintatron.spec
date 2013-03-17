#
# rpm spec for paintatron
#

%define        __spec_install_post %{nil}
%define          debug_package %{nil}
%define        __os_install_post %{_dbpath}/brp-compress

Summary: Generates artwork using genetic programming 
Name: paintatron
Version: 1.00
Release: 1
License: GPL
Group: Utilities
SOURCE0 : %{name}-%{version}.tar.gz
URL: https://github.com/fuzzgun/paintatron
Packager: Bob Mottram <bob@sluggish.dyndns.org>
Requires: gnuplot, libpng, qt4, libgpr
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
Paintatron can be used to generate artworks based upon a set of source images. The user repeatedly selects the most aesthetic images from which the next generation of images are produced.

%prep
%setup -q

%build
 # Empty section.

%install
rm -rf %{buildroot}
mkdir -p  %{buildroot}

# in builddir
cp -a * %{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%config(noreplace) %{_sysconfdir}/%{name}/%{name}.conf
%{_bindir}/*
%attr(644,root,root) /usr/share/man/man1/paintatron.1.gz
%attr(644,root,root) /usr/share/applications/%{name}/%{name}.desktop
%attr(644,root,root) /usr/share/icons/hicolor/24x24/apps/%{name}.png
%attr(644,root,root) /usr/share/icons/hicolor/scalable/apps/%{name}.svg
%attr(644,root,root) /usr/share/pixmaps/%{name}.svg

%changelog
* Sun Mar 17 2013  Bob Mottram <bob@sluggish.dyndns.org> 0.2-1
- Spec file created

