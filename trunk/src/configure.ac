AC_INIT([getodo],[0.1],[bohumir@zamecnik.org])
AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CXX
AC_PROG_CC
AX_BOOST_BASE([1.33])
AX_BOOST_DATE_TIME

# create a config.h file (Automake will add -DHAVE_CONFIG_H)
AC_CONFIG_SRCDIR([getodo.cpp])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
