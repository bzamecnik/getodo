noinst_LIBRARIES = libgetodo.a
libgetodo_a_SOURCES = getodo.h getodo.cpp
# could be: -L../../lib/sqlite3x/ -lsqlite3x ?
libgetodo_a_LIBADD = ../../lib/sqlite3x/libsqlite3x.so
INCLUDES = -I@top_srcdir@/lib/
AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)
