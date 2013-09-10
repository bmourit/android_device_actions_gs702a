# Process this file with autoconf to produce a configure script.
AC_INIT([makebootfat], [1.4])
AM_INIT_AUTOMAKE([foreign no-dependencies])
AC_CONFIG_SRCDIR([makebootfat.c])
AC_CONFIG_HEADER([config.h])
AC_CANONICAL_HOST
AC_PREREQ(2.57)
AC_COPYRIGHT([Copyright 2005 Andrea Mazzoleni])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_SYS_LARGEFILE

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_HEADER_TIME
AC_CHECK_HEADERS([unistd.h getopt.h stdarg.h varargs.h])
AC_CHECK_HEADERS([sys/types.h sys/stat.h sys/time.h sys/ioctl.h])
AC_CHECK_HEADERS([scsi/scsi.h mntent.h linux/hdreg.h linux/fd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([getopt getopt_long snprintf vsnprintf])
AC_CHECK_FUNCS([setmntent getmntent endmntent])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

