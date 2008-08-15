#!/usr/bin/make -f
# Requires GNU Make!

# Removed sq3 stuff, we need only sqlite3x. -- BZ

PACKAGE.NAME = libsqlite3x
PACKAGE.VERSION = 2007.10.18

SQLITE3_INCLUDES ?= -I$(HOME)/include
SQLITE3_LDFLAGS ?= -L$(HOME)/lib -lsqlite3

CXXFLAGS ?= -g -Wall -Werror
INCLUDES += $(SQLITE3_INCLUDES)

include shake-n-make.make

########################################################################
# sqlite3x:
SQLITE3X.SOURCES = sqlite3x_command.cpp \
	sqlite3x_connection.cpp \
	sqlite3x_exception.cpp \
	sqlite3x_cursor.cpp \
	sqlite3x_transaction.cpp \
	sqlite3x_settings_db.cpp

SQLITE3X.HEADERS = sqlite3x.hpp \
	sqlite3x_settings_db.hpp

SQLITE3X.OBJECTS = $(patsubst %.cpp,%.o,$(SQLITE3X.SOURCES))
CLEAN_FILES += $(SQ3LITE3X.OBJECTS)

libsqlite3x.LIB.OBJECTS = $(SQLITE3X.OBJECTS)
libsqlite3x.DLL.OBJECTS = $(libsqlite3x.LIB.OBJECTS)
libsqlite3x.DLL.LDFLAGS = $(SQLITE3_LDFLAGS)
$(call ShakeNMake.CALL.RULES.DLLS,libsqlite3x)
$(call ShakeNMake.CALL.RULES.LIBS,libsqlite3x)
# end sqlite3x
########################################################################

dist-pre: doxygen-dist

########################################################################
# Purely administrative stuff:
PACKAGE.DIST_FILES += ChangeLog README \
	$(SQLITE3X.SOURCES) $(SQLITE3X.HEADERS)
CLEAN_FILES += *~
libs: libsqlite3x.LIB libsqlite3x.DLL
########################################################################

all: libs
