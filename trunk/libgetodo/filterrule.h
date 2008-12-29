// $Id$
//
// C++ Interface: FilterRule
//
// Description: 
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_FILTERRULE_H
#define LIBGETODO_FILTERRULE_H

#include "common.h"
#include <sstream>

using namespace sqlite3x;

namespace getodo {

/** Filter rule.
 * Rule for filtering tasks.
 */
class FilterRule {
public:
	id_t id; // -1, if not already in database
	std::string name;
	std::string rule;

	FilterRule();
	FilterRule(const FilterRule& r);
	FilterRule(std::string name, std::string rule);
	FilterRule(id_t id, std::string name, std::string rule);
	virtual ~FilterRule();

	std::string toString() const;
	friend std::ostream& operator<< (std::ostream& o, const FilterRule& rule);

	static const id_t INVALID_ID = -1;
	static bool isValidId(id_t id);
	bool hasValidId() const;
};

/** Filter rule persistence.
 * Object-relation mapping of filter rules.
 */
class FilterRulePersistence {
private:
	sqlite3_connection* conn;
public:
	// constructor for loading new FilterRules
	FilterRulePersistence(sqlite3_connection* conn);
	virtual ~FilterRulePersistence();

	// save FilterRule to database
	// - if it has no id (eg. it was newly created), assign some
	void save(FilterRule& filter);
	// load FilterRule from database
	void load(FilterRule& filter, id_t id);
	
	void erase(id_t id);

	void setName(FilterRule& filter, const std::string name);
	void setRule(FilterRule& filter, const std::string rule);
private:
	void setColumn(id_t id, const std::string value, const std::string column);
};

} // namespace getodo

#endif // LIBGETODO_FILTERRULE_H