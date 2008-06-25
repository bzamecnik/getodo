ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = getodo
getodo_SOURCES = getodo.cpp main.cpp
getodo_LDADD = $(BOOST_DATE_TIME_LIB) -Lsqlite3x -lsqlite3 -lsqlite3x
AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)

