# options and settings
PROJECT	?= nypack_test

# app staticlib sharedlib
TEMPLATE ?= app

#
# available OPT:
# 0 - 3: low optimization - high optimization
# s: size optimization
# fast
OPT	= s

USE_STATIC_BUILD = no
USE_DEBUG = no

# list options
USE_LIST_OUTPUT = no

# directory define
PRJDIR   = .
SRCDIR   = $(PRJDIR)/src
OBJ_DIR ?= obj

# User source files/includes/macros
# add .c source files to USR_SRCS
# add .h header files to USR_INCS
# add pre-defines to USR_DEFS
# add library directory to USR_LIBS
USR_DEFS +=

# cross compile
#TCHAIN   ?= arm-none-eabi-
#SYSROOT  ?= /usr
#USR_INCS += ${SYSROOT}/include

# pkg-config packages depends on
pkg_packages :=

USR_LIBS += -lpthread -ldl -lm -lstdc++

# nypack
VPATH    += $(SRCDIR)
USR_INCS += $(PRJDIR)/inc
USR_SRCS += $(wildcard $(SRCDIR)/*.c)

# test
SUBDIR_TEST = $(PRJDIR)/test
VPATH    += $(SUBDIR_TEST)
USR_INCS += $(SUBDIR_TEST)
USR_SRCS += nypack_test.c

SUBDIR_CMOCKA = $(PRJDIR)/test/cmocka
VPATH    += $(SUBDIR_CMOCKA)/src
USR_INCS += $(SUBDIR_CMOCKA)/include
USR_SRCS += cmocka.c
USR_DEFS  += -DHAVE_SIGNAL_H -DHAVE_MALLOC_H

# include common
include common.mk
