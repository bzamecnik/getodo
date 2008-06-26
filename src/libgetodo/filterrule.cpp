// $Id:$
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

FilterRule::FilterRule() : filterRuleId(-1) {}

FilterRule::FilterRule(std::string name, std::string rule)
	: filterRuleId(-1) {
	this->name = name;
	this->rule = rule;
}

FilterRule::FilterRule(id_t filterRuleId, std::string name, std::string rule) {
	this->filterRuleId = filterRuleId;
	this->name = name;
	this->rule = rule;
}

// ----- class FilterRulePersistence --------------------

FilterRulePersistence::FilterRulePersistence(sqlite3_connection* conn) {
	this->conn = conn;
}

FilterRule& FilterRulePersistence::save(FilterRule& filterRule) {
	// if(!conn) { TODO: throw ...}
	int count = 0;
	if (filterRule.filterRuleId >= 0) {
		// find out, if there is already a filterRule with such filterRuleID
		sqlite3_command cmd(*conn, "SELECT count(*) FROM FilterRule WHERE filterRuleId = (?);");
		cmd.bind(1, filterRule.filterRuleId);
		count = cmd.executeint();
	}
	if (count > 0) {
		// it is already there -> update
		sqlite3_command cmd(*conn, "UPDATE FilterRule SET name = ?, rule = ? WHERE filterRuleId = (?);");
		cmd.bind(1, filterRule.name);
		cmd.bind(2, filterRule.name);
		cmd.bind(3, filterRule.filterRuleId);
		cmd.executenonquery();
	} else {
		// it is not there -> insert
		sqlite3_command cmd(*conn, "INSERT INTO FilterRule (name,rule) VALUES (?,?);");
		cmd.bind(1, filterRule.name);
		cmd.bind(2, filterRule.rule);
		cmd.executenonquery();
		// get filterRuleId which database automatically created
		filterRule.filterRuleId = sqlite3_last_insert_rowid(conn->db());
	}
	return filterRule;
}

FilterRule& FilterRulePersistence::load(id_t filterRuleId) {
	// if(!conn) { TODO: throw ...}

	sqlite3_command cmd(*conn, "SELECT name,rule FROM FilterRule WHERE filterRuleId = (?);");
	cmd.bind(1, filterRuleId);
	sqlite3_cursor cur = cmd.executecursor();
	std::string name, rule;
	if (cur.step()) {
		name = cur.getstring(1);
		rule = cur.getstring(2);
	}
	// TODO: throw, if there is not record  with this filterRuleID
	cur.close();
	return *(new FilterRule(filterRuleId, name, rule));
}

} // namespace getodo
