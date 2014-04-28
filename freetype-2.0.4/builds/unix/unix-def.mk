#
# FreeType 2 configuration rules templates for Unix + configure
#


# Copyright 1996-2000 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.


ifndef TOP
  TOP := .
endif
TOP := $(shell cd $(TOP); pwd)

DELETE        := rm -f
DELDIR        := rmdir
SEP           := /
HOSTSEP       := $(SEP)
BUILD         := $(TOP)/builds/unix
PLATFORM      := unix

# don't use `:=' here since the path stuff will be included after this file
#
FTSYS_SRC = $(BUILD)/ftsystem.c

INSTALL         := /usr/bin/install -c
INSTALL_DATA    := ${INSTALL} -m 644
INSTALL_PROGRAM := ${INSTALL}
INSTALL_SCRIPT  := ${INSTALL}
MKINSTALLDIRS   := $(BUILD)/mkinstalldirs

DISTCLEAN += $(BUILD)/config.cache    \
             $(BUILD)/config.log      \
             $(BUILD)/config.status   \
             $(BUILD)/unix-def.mk     \
             $(BUILD)/unix-cc.mk      \
             $(BUILD)/ftconfig.h      \
             $(BUILD)/freetype-config \
             $(LIBTOOL)


# Standard installation variables.
#
prefix       := /usr/local
exec_prefix  := ${prefix}
libdir       := ${exec_prefix}/lib
bindir       := ${exec_prefix}/bin
includedir   := ${prefix}/include

version_info := 7:0:1


# The directory where all object files are placed.
#
# This lets you build the library in your own directory with something like
#
#   set TOP=.../path/to/freetype2/top/dir...
#   set OBJ_DIR=.../path/to/obj/dir
#   make -f $TOP/Makefile setup [options]
#   make -f $TOP/Makefile
#
ifndef OBJ_DIR
  OBJ_DIR := $(shell cd $(TOP)/objs; pwd)
endif


# The directory where all library files are placed.
#
# By default, this is the same as $(OBJ_DIR); however, this can be changed
# to suit particular needs.
#
LIB_DIR := $(OBJ_DIR)


# The NO_OUTPUT macro is appended to command lines in order to ignore
# the output of some programs.
#
NO_OUTPUT := 2> /dev/null

# EOF
