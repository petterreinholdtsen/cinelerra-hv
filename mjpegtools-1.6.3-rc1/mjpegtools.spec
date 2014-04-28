%define name    mjpegtools
%define version 1.6.3-rc1
%define release 1

%define infoentry "* mjpeg-howto: (mjpeg-howto).        How to use the mjpeg tools"
%define infofile mjpeg-howto.info

%define __os_install_post %{nil}

Name:           %name
Version:        %version
Release:        %release
Summary:	Tools for recording, editing, playing back and mpeg-encoding video under linux
License:	GPL
Url:		http://mjpeg.sourceforge.net/
Group:		Video
Source0:	http://prdownloads.sourceforge.net/mjpeg/mjpegtools-%{version}.tar.gz
Source1:	http://prdownloads.sourceforge.net/mjpeg/jpeg-mmx-0.1.6.tar.gz
BuildRoot:      %{_tmppath}/%{name}-buildroot-%{version}-%{release}

Requires:	XFree86 SDL
Requires:	libpng libjpeg
Requires:	glib
Requires:	libquicktime
Requires:	libdv

BuildRequires:  XFree86-devel SDL-devel
BuildRequires:  libpng-devel libjpeg-devel
BuildRequires:  glib-devel gtk+-devel
BuildRequires:  gcc-c++
BuildRequires:	libquicktime-devel
BuildRequires:	libdv-devel

Prefix:		%{_prefix}

%description
The MJPEG-tools are a basic set of utilities for recording, editing, 
playing back and encoding (to mpeg) video under linux. Recording can
be done with zoran-based MJPEG-boards (LML33, Iomega Buz, Pinnacle
DC10(+), Marvel G200/G400), these can also playback video using the
hardware. With the rest of the tools, this video can be edited and
encoded into mpeg1/2 or divx video.

%prep
%setup -q -n %{name}-%{version} -a 1

mv jpeg-mmx-* jpeg-mmx

mkdir usr

%build

tmp_prefix="`pwd`/usr"
mkdir -p $tmp_prefix/{include,lib,bin,share}

CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS
CXXFLAGS="${CXXFLAGS:-%optflags}" ; export CXXFLAGS

(cd jpeg-mmx && ./configure)
make -C jpeg-mmx libjpeg-mmx.a

CONF_ARGS="--with-jpeg-mmx=`pwd`/jpeg-mmx"

./configure \
	$CONF_ARGS \
	--enable-large-file \
	--prefix=%{_prefix}
make

%install
[ -n "${RPM_BUILD_ROOT}" -a "${RPM_BUILD_ROOT}" != / ] \
 && rm -rf ${RPM_BUILD_ROOT}/

make prefix=${RPM_BUILD_ROOT}%{prefix} INSTALL_INFO= install

%post
/sbin/install-info \
	--entry=%{infoentry} \
	--info-dir=%{_prefix}/info \
	%{_prefix}/info/%{infofile}
/sbin/ldconfig

%postun
/sbin/install-info \
	--remove \
	--info-dir=%{_prefix}/info \
	%{_prefix}/info/%{infofile}

%clean
[ -n "${RPM_BUILD_ROOT}" -a "${RPM_BUILD_ROOT}" != / ] \
 && rm -rf ${RPM_BUILD_ROOT}/

%files
%defattr(-,root,root)
%doc AUTHORS BUGS CHANGES COPYING HINTS PLANS README TODO
%{_bindir}/lav*
%{_bindir}/yuv*
%{_bindir}/jpeg2yuv
%{_bindir}/testrec
%{_bindir}/y4m*
%{_bindir}/ppm*
%{_bindir}/glav
%{_bindir}/ypipe
%{_bindir}/mp*
%{_bindir}/pgmtoy4m
%{_bindir}/png2yuv
%{_bindir}/*.flt
%{_libdir}/*.so.*
%{_prefix}/man/man1/*
%{_prefix}/man/man5/*
%{_prefix}/info/

%package devel
Summary: Development headers and libraries for the mjpegtools
Group: Development/Libraries

%description devel
This package contains static libraries and C system header files
needed to compile applications that use part of the libraries
of the mjpegtools package.

%files devel
%{_bindir}/*-config
%{_includedir}/mjpegtools/*.h
%{_includedir}/mjpegtools/mpeg2enc/*.hh
%{_includedir}/mjpegtools/mpeg2enc/*.h
%{_includedir}/mjpegtools/mplex/*.hpp
%{_libdir}/pkgconfig/*.pc
%{_libdir}/*.a
%{_libdir}/*.la
%{_libdir}/*.so

%changelog
* Fri Dec 19 2003 Ronald Bultje <rbultje@ronald.bitfreak.net>
- add everything for mpeg2enc/mplex libs (dev headers and so on)

* Sat Aug 23 2003 Ronald Bultje <rbultje@ronald.bitfreak.net>
- Remove quicktime4linux hacks, add libquicktime depdency
- Remove avifile leftovers

* Wed May 20 2002 Thomas Vander Stichele <thomas@apestaart.org>
- Added BuildRequires and Requires

* Tue Feb 12 2002 Geoffrey T. Dairiki <dairiki@dairiki.org>
- Fix spec file to build in one directory, etc...

* Thu Dec 06 2001 Ronald Bultje <rbultje@ronald.bitfreak.net>
- separated mjpegtools and mjpegtools-devel
- added changes by Marcel Pol <mpol@gmx.net> for cleaner RPM build

* Wed Jun 06 2001 Ronald Bultje <rbultje@ronald.bitfreak.net>
- 1.4.0-final release, including precompiled binaries (deb/rpm)
