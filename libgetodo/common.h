#ifndef LIBGETODO_COMMON_H
#define LIBGETODO_COMMON_H

#include <list>
#include <map>
#include <set>
#include <string>
#include "sqlite3x/sqlite3x.hpp"
#include "datetime.h"

namespace getodo {

typedef int id_t;
typedef std::map<std::string,std::string> databaseRow_t;

}

#endif // LIBGETODO_COMMON_H
