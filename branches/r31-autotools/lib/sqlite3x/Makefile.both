#!/usr/bin/make -f
# Requires GNU Make!

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

########################################################################
# sq3:
SQ3.SOURCES = \
	sq3.cpp \
	sq3_log_db.cpp \
	sq3_settings_db.cpp

SQ3.HEADERS = \
	refcount.hpp \
	sq3.hpp \
	sq3_log_db.hpp \
	sq3_settings_db.hpp

SQ3.OBJECTS = $(patsubst %.cpp,%.o,$(SQ3.SOURCES))
CLEAN_FILES += $(SQ3.OBJECTS)

libsq3.LIB.OBJECTS = $(SQ3.OBJECTS)
libsq3.DLL.OBJECTS = $(libsq3.LIB.OBJECTS)
libsq3.DLL.LDFLAGS = $(SQLITE3_LDFLAGS)
$(call ShakeNMake.CALL.RULES.DLLS,libsq3)
$(call ShakeNMake.CALL.RULES.LIBS,libsq3)
# end sq3
########################################################################

########################################################################
# bins
test.BIN.OBJECTS = test.o
test.OBJ.CPPFLAGS = $(SQLITE3_INCLUDES)
test.BIN.LDFLAGS = -lstdc++ -L. -lsq3 -lsqlite3x
$(call ShakeNMake.CALL.RULES.BINS,test)
$(test.BIN): $(libsq3.DLL) $(libsqlite3x.DLL) $(MAKEFILE_LIST)
run: bins
	LD_LIBRARY_PATH=".:$${LD_LIBRARY_PATH}" ./$(test.BIN)
# $(SQLITE3_LDFLAGS)
# end bins
########################################################################

dist-pre: doxygen-dist

########################################################################
# Purely administrative stuff:
PACKAGE.DIST_FILES += ChangeLog README \
	$(SQLITE3X.SOURCES) $(SQLITE3X.HEADERS) \
	$(SQ3.SOURCES) $(SQ3.HEADERS) \
	test.cpp
CLEAN_FILES += *~
libs: libsqlite3x.LIB libsqlite3x.DLL libsq3.LIB libsq3.DLL
bins: test.BIN
########################################################################

all: libs bins
