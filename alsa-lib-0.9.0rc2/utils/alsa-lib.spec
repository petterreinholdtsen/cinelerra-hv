%define ver      0.9.0rc2
%define rel      1

Summary: Advanced Linux Sound Architecture (ALSA) - Library
Name: alsa-lib
Version: %ver
Release: %rel
Copyright: GPL
Group: System/Libraries
Source: ftp://ftp.alsa-project.org/pub/lib/alsa-lib-%{ver}.tar.bz2
BuildRoot: /var/tmp/alsa-lib-%{ver}
URL: http://www.alsa-project.org
Requires: alsa-driver

%description

Advanced Linux Sound Architecture (ALSA) - Library

%changelog

* Tue Nov 20 2001 Jaroslav Kysela <perex@suse.cz>

- changed BuildRoot from /tmp to /var/tmp
- use the standard RPM macros for prefix and paths
- added DESTDIR for make install

* Sun Nov 11 2001 Miroslav Benes <mbenes@tenez.cz>

- dangerous command "rpm -rf $RPM_BUILD_ROOT" checks $RPM_BUILD_ROOT variable
- unset key "Docdir" - on some new systems are documentation in /usr/share/doc

* Mon May 28 1998 Helge Jensen <slog@slog.dk>

- Made SPEC file

%prep
%setup
%build

./configure --prefix=%_prefix --mandir=%_mandir --datadir=%_datadir \
	    --includedir=%_includedir --libdir=%_libdir
make

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT/%_includedir/alsa
mkdir -p $RPM_BUILD_ROOT/%_datadir/alsa
mkdir -p $RPM_BUILD_ROOT/%_datadir/aclocal
mkdir -p $RPM_BUILD_ROOT/%_libdir

make DESTDIR="$RPM_BUILD_ROOT" install

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%doc doc/*.txt

%_includedir/alsa/*.h
%_libdir/lib*
%_datadir/aclocal/alsa.m4
%_datadir/alsa/*
%_prefix/bin/aserver