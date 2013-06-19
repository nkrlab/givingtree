dnl Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
dnl
dnl This work is confidential and proprietary to Nexon Korea Corporation and
dnl must not be used, disclosed, copied, or distributed without the prior
dnl consent of Nexon Korea Corporation.

dnl Process this file with autoconf to produce a configure script.

dnl Customizable variables are
dnl   giving_tree : project name

AC_PREREQ(2.67)
AC_INIT(giving_tree, 0.0.1,)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_AUX_DIR(autoconf-aux)
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADER([config.h])
AC_PREFIX_DEFAULT(["/usr"])
AC_LANG([C++])

AC_SUBST([PACKAGE_DEB], [`echo $PACKAGE_NAME | sed -e 's/_/-/g'`])

AM_INIT_AUTOMAKE()

dnl Checks required compilers and tools.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

dnl Uses gcc extentions and allows files larger than 2GB.
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE

dnl Prefers libtool shared libs.
LT_PREREQ([2.2])
LT_INIT([dlopen disable-static])
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

dnl Checks required libraries.
dnl Boost sets own variable for library location.
dnl Thus we add such variable to LIBS in Makefile.am.
AX_BOOST_BASE([1.46], , AC_MSG_ERROR([Boost is required]))
AX_FUNAPI([0.0.1], , AC_MSG_ERROR([FunAPI is required]))
AX_PROTOBUF

dnl AC_CHECK_LIB adds to LIBS if action-found is missing.
AC_CHECK_HEADERS(glog/logging.h, , AC_MSG_ERROR([glog is required]))


dnl Optional features. Don't have to stop even if not exist.
AX_CHECK_COVERAGE
AX_CHECK_DEBUGGING
AX_CHECK_PROFILING

AC_CONFIG_FILES([
Makefile
])
AC_OUTPUT
