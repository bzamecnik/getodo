// $Id:$
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

#include "getodo.h"

namespace getodo {

class FilterRule {
	id_t filterRuleId;
	std::string name;
	std::string rule;
public:
	FilterRule(std::string name, std::string rule);
	~FilterRule();

	id_t getFilterRuleId();

	std::string getName();
	void setName(const std::string name);
	
	std::string getRule();
	void setRule(const std::string rule);
};

class FilterRulePersistence {
	sqlite3x::sqlite3_connection* db;
public:
	// Constructor for loading new FilterRules
	FilterRulePersistence(sqlite3x::sqlite3_connection* db);

	// save FilterRule to database
	void save(const FilterRule& filterRule);
	// load FilterRule from database
	FilterRule& load(id_t filterRuleId);

	void setName(const std::string name);
	void setRule(const std::string rule);
};

} // namespace getodo
