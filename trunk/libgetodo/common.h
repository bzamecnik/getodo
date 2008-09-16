// $Id$
//
// Common includes and typedefs.
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef LIBGETODO_COMMON_H
#define LIBGETODO_COMMON_H

#include <list>
#include <map>
#include <set>
#include <string>
#include "sqlite3x/sqlite3x.hpp"
#include "datetime.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace getodo {

typedef int id_t;
typedef std::map<std::string,std::string> databaseRow_t;
// TODO: rename to idset_t, use for tags, subtasks
typedef std::set<id_t> taskset_t;

class Task;
class TaskPersistence;
class TaskManager;

// ----- utils --------------------

// Join items with "join_with"
// eg:
// int list_init[4] = {42,17,68,89};
// join(std::cout,&list_init[0],&list_init[4], ", ");
//
// std::list<int> alist(&list_init[0],&list_init[4]);
// join(std::cout,alist.begin(), alist.end(), ", ");
// Thanks to Thomas Guest:
// http://wordaligned.org/articles/joined-output-and-the-fencepost-problem

template <class Iterator>
void join(std::ostream & output,
      Iterator first,
      Iterator last,
      char const * join_with)
{
    if (first != last) {
        Iterator curr = first++;
        while (first != last) {
            output << *curr << join_with;
            curr = first++;
        }
        output << *curr;
    }
}

}

#endif // LIBGETODO_COMMON_H
