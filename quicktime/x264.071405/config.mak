prefix=/usr/local
eprefix=${prefix}
bindir=${eprefix}/bin
libdir=${eprefix}/lib
includedir=${prefix}/include
ARCH=X86_64
SYS=LINUX
CC=gcc
CFLAGS=-Wall -I. -O4 -ffast-math -D__X264__ -DHAVE_MALLOC_H -DHAVE_MMXEXT -DHAVE_SSE2 -DARCH_X86_64 -DSYS_LINUX -DHAVE_PTHREAD=1 -s -fomit-frame-pointer
LDFLAGS= -lm -lpthread -s
AS=yasm
ASFLAGS=-f elf -m amd64
VFW=no
EXE=
VIS=no
