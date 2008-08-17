# ./configure invokation with Boost and SQLite in non-standard locations
CPPFLAGS=-I/usr/local/include ./configure \
  --with-boost-libdir=/usr/local/lib \
  --with-sqlite3=/usr/local
