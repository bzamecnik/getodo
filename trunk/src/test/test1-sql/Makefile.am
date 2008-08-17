bin_PROGRAMS = test1
test1_SOURCES = test1.cpp
noinst_HEADERS = test1.h
test1_LDADD = ../../libgetodo/libgetodo.a \
  ../../../lib/sqlite3x/libsqlite3x.a \
  @SQLITE3_LDFLAGS@ @BOOST_DATE_TIME_LIB@
INCLUDES = -I@top_srcdir@/src/libgetodo -I@top_srcdir@/lib
AM_CXXFLAGS = @SQLITE3_CFLAGS@ $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)
