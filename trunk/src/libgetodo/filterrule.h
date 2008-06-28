// $Id$
//
// C++ Interface: FilterRule
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_FILTERRULE_H
#define LIBGETODO_FILTERRULE_H

#include "getodo.h"

using namespace sqlite3x;

namespace getodo {

class FilterRule {
public:
	id_t filterRuleId; // -1, if not already in database
	std::string name;
	std::string rule;

	FilterRule();
	FilterRule(const FilterRule& r);
	FilterRule(std::string name, std::string rule);
	FilterRule(id_t filterRuleId, std::string name, std::string rule);
	~FilterRule();
};

class FilterRulePersistence {
private:
	sqlite3_connection* conn;
public:
	// constructor for loading new FilterRules
	FilterRulePersistence(sqlite3_connection* conn);

	// save FilterRule to database
	// - if it has no filterRuleId (eg. it was newly created), assign some
	FilterRule& save(FilterRule& filterRule);
	// load FilterRule from database
	FilterRule& load(id_t filterRuleId);
	
	void erase(id_t filterRuleId);

	void setName(const std::string name);
	void setRule(const std::string rule);
};

} // namespace getodo

#endif // LIBGETODO_FILTERRULE_H
