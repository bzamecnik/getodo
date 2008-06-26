noinst_LIBRARIES = libgetodo.a
libgetodo_a_SOURCES = filterrule.cpp getodo.cpp tag.cpp task.cpp \
	taskmanager.cpp
noinst_HEADERS = filterrule.h getodo.h tag.h task.h taskmanager.h
# could be: -L../../lib/sqlite3x/ -lsqlite3x ?
libgetodo_a_LIBADD = ../../lib/sqlite3x/libsqlite3x.so
INCLUDES = -I@top_srcdir@/lib/
AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)

