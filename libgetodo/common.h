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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>

#include "sqlite3x/sqlite3x.hpp"
#include "datetime.h"
#include "recurrence.h"

#define foreach BOOST_FOREACH

/** GeToDo namespace. */
namespace getodo {

/** Identification of tasks, tags, ... */
typedef int id_t;
/** Set of id's of tasks, tags, ... whatever using id_t */
typedef std::set<id_t> idset_t;
/** Database row type.
 * Key is database column name, value is its value.
 * Used for example for (de)serialization of tasks.
 */
typedef std::map<std::string,std::string> databaseRow_t;

class Task;
class TaskPersistence;
class TaskManager;

// ----- exceptions ---------------

/** GeToDo Error exception.
 * A generic runtime exception specific to GeToDo application.
 */
class GetodoError : public std::runtime_error {
public:
	GetodoError() : std::runtime_error("Unknown error") {}
	GetodoError(const std::string& msg) : std::runtime_error(msg) {}
};

// TODO: make more exceptions for specific purposes

// ----- utils --------------------

/** Join items in a range with a separator.
 * The separator goes only between the items, neither at the beginning nor the end.
 *
 * The items must have an operator<<().
 *
 * Example:
 * int list_init[4] = {42,17,68,89};
 * join(std::cout,&list_init[0],&list_init[4], ", ");
 *
 * std::list<int> alist(&list_init[0],&list_init[4]);
 * join(std::cout,alist.begin(), alist.end(), ", ");
 *
 * Thanks to Thomas Guest:
 * http://wordaligned.org/articles/joined-output-and-the-fencepost-problem
 *
 * \param output output stream
 * \param begin iterator pointing to the first item
 * \param end iterator pointing after the last item
 * \param separator string which goes between the items
 */
template <class Iterator>
void join(std::ostream & output,
      Iterator begin,
      Iterator end,
      char const * separator)
{
    if (begin != end) {
        Iterator current = begin++;
        while (begin != end) {
            output << *current << separator;
            current = begin++;
        }
        output << *current;
    }
}

/** Escape an SQLite query string.
 * Replace single apostrophes with double ones.
 * \param input SQLite query string
 * \return escaped query string
 */
std::string sqliteEscapeString(const std::string& input);
/** Unescape an SQLite query string.
 * Replace double apostrophes with single ones.
 * \param input SQLite query string
 * \return unescaped query string
 */
std::string sqliteUnescapeString(const std::string& input);

} // namespace getodo

#endif // LIBGETODO_COMMON_H
