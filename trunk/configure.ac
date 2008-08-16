AC_INIT([getodo],[0.1],[bohumir@zamecnik.org])
AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR(config)
# Don't let automake complain about missing files:
#   NEWS README AUTHORS ChangeLog
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
#AC_PROG_INSTALL

AX_BOOST_BASE([1.33])
AX_BOOST_DATE_TIME
# find SQLite and set @SQLITE3_LDFLAGS@ and @SQLITE3_CFLAGS@
AX_LIB_SQLITE3([3.0.0])
## otherwise can be used this (but is not so mighty)
#AC_CHECK_LIB([sqlite3],[main])

## create a config.h file (Automake will add -DHAVE_CONFIG_H)
#AC_CONFIG_SRCDIR([src/libgetodo/getodo.cpp])
#AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile \
	lib/Makefile lib/sqlite3x/Makefile \
	src/Makefile src/libgetodo/Makefile src/getodogui/Makefile \
	src/test/Makefile src/test/test1-sql/Makefile])
AC_OUTPUT
