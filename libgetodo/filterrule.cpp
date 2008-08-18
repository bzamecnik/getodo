// $Id$
//
// C++ Implementation: filterrule
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "filterrule.h"

namespace getodo {

// ----- class FilterRule --------------------

FilterRule::FilterRule()
	: filterRuleId(-1) {}
FilterRule::FilterRule(const FilterRule& r)
	: filterRuleId(r.filterRuleId), name(r.name), rule(r.name) {}
FilterRule::FilterRule(std::string n, std::string r)
	: filterRuleId(-1), name(n), rule(r) {}
FilterRule::FilterRule(id_t id, std::string n, std::string r)
	: filterRuleId(id), name(n), rule(r) {}
FilterRule::~FilterRule() {}

// ----- class FilterRulePersistence --------------------

FilterRulePersistence::FilterRulePersistence(sqlite3_connection* c)
	: conn(c) {}

FilterRulePersistence::~FilterRulePersistence() {}

void FilterRulePersistence::save(FilterRule& filterRule) {
	// if(!conn) { TODO: throw ...}
	int count = 0;
	if (filterRule.filterRuleId >= 0) {
		// find out, if there is already a filterRule with such a filterRuleID
		sqlite3_command cmd(*conn, "SELECT count(*) FROM FilterRule WHERE filterRuleId = ?;");
		cmd.bind(1, filterRule.filterRuleId);
		count = cmd.executeint();
	}
	//// uncomment this if filterRuleName is UNIQUE
	//else {	
	//	sqlite3_command cmd(*conn, "SELECT filterRuleId FROM FilterRule WHERE FilterRuleName = ?;");
	//	cmd.bind(1, filterRule.filterRuleName);
	//	sqlite3_cursor cursor = cmd.executecursor();
	//	if (cursor.step()) {
	//		// if there is already a filterRule with such filterRuleName, set the
	//		// filterRule.filterRuleId respectively
	//		filterRule.filterRuleId = cursor.getint(0);
	//		cursor.close();
	//		return;
	//	}
	//  cursor.close();
	//}
	if (count > 0) {
		if (filterRule.filterRuleId >= 0) {
			// it is already there -> update
			sqlite3_command cmd(*conn, "UPDATE FilterRule SET name = ?, rule = ? WHERE filterRuleId = ?;");
			cmd.bind(1, filterRule.name);
			cmd.bind(2, filterRule.name);
			cmd.bind(3, filterRule.filterRuleId);
			cmd.executenonquery();
		}
	} else {
		// it is not there -> insert
		sqlite3_command cmd(*conn, "INSERT INTO FilterRule (name,rule) VALUES (?,?);");
		cmd.bind(1, filterRule.name);
		cmd.bind(2, filterRule.rule);
		cmd.executenonquery();
		// get filterRuleId which database automatically created
		filterRule.filterRuleId = sqlite3_last_insert_rowid(conn->db());
	}
}

FilterRule& FilterRulePersistence::load(id_t filterRuleId) {
	// if(!conn) { TODO: throw ...}

	sqlite3_command cmd(*conn, "SELECT name,rule FROM FilterRule WHERE filterRuleId = ?;");
	cmd.bind(1, filterRuleId);
	sqlite3_cursor cursor = cmd.executecursor();
	std::string name, rule;
	if (cursor.step()) {
		name = cursor.getstring(0);
		rule = cursor.getstring(1);
	}
	// TODO: throw, if there is not record  with this filterRuleID
	cursor.close();
	return *(new FilterRule(filterRuleId, name, rule));
}

void FilterRulePersistence::erase(id_t filterRuleId) {
	// if(!conn) { TODO: throw ...}
	
	sqlite3_command cmd(*conn, "DELETE FROM FilterRule WHERE filterRuleId = ?;");
	cmd.bind(1, filterRuleId);
	cmd.executenonquery();
}

void FilterRulePersistence::setName(id_t filterRuleId, const std::string name) {
	setColumn(filterRuleId, name, "name");
}

void FilterRulePersistence::setRule(id_t filterRuleId, const std::string rule) {
	setColumn(filterRuleId, rule, "rule");
}
void FilterRulePersistence::setColumn(id_t filterRuleId, const std::string value, const std::string column) {
	// if(!conn) { TODO: throw ...}
	
	// TODO: check, if the filter rule really exixsts in database, else throw
	
	sqlite3_command cmd(*conn, "UPDATE FilterRule SET (?) VALUES (?) WHERE filterRuleId = ?;");
	cmd.bind(1, column);
	cmd.bind(1, value);
	cmd.bind(2, filterRuleId);
	cmd.executenonquery();
}

} // namespace getodo
