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
	: id(INVALID_ID) {}
FilterRule::FilterRule(const FilterRule& r)
	: id(r.id), name(r.name), rule(r.rule) {}
FilterRule::FilterRule(std::string n, std::string r)
	: id(INVALID_ID), name(n), rule(r) {}
FilterRule::FilterRule(id_t id, std::string n, std::string r)
	: id(id), name(n), rule(r) {}
FilterRule::~FilterRule() {}


idset_t& FilterRule::filter(sqlite3_connection& conn) {
	// filter using a SQL query
	idset_t& tasksOk = *(new idset_t());

	try {
		sqlite3_command cmd(conn, rule + ";");
		sqlite3_cursor cursor = cmd.executecursor();
		while (cursor.step()) {
			tasksOk.insert(tasksOk.end(), cursor.getint(0));
		}
		cursor.close();
	} catch (sqlite3x::database_error&) {
		// throw an exception if the query is broken
	}

	return tasksOk;
}


std::string FilterRule::toString() const {
	return boost::lexical_cast<std::string, FilterRule>(*this);
}
std::ostream& operator<< (std::ostream& o, const FilterRule& rule) {
	// eg.: FilterRule [id, "name"]: "fitler rule"
	o << "FilterRule [";
	o << rule.id << ", \"" << rule.name << "\"]: \"";
	o << rule.rule << '"';
	return o;
}

bool FilterRule::isValidId(id_t id) {
	return id >= 0;
}

bool FilterRule::hasValidId() const {
	return FilterRule::isValidId(id);
}

bool FilterRule::isEmpty() const {
	return rule.empty();
}

// ----- class FilterRulePersistence --------------------

FilterRulePersistence::FilterRulePersistence(sqlite3_connection* c)
	: conn(c) {}

FilterRulePersistence::~FilterRulePersistence() {}

void FilterRulePersistence::save(FilterRule& filter) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }

	int count = 0;
	if (filter.id >= 0) {
		// find out, if there is already a filter with such a filterRuleId
		sqlite3_command cmd(*conn, "SELECT count(*) FROM FilterRule WHERE filterRuleId = ?;");
		cmd.bind(1, filter.id);
		count = cmd.executeint();
	}
	//// uncomment this if filterRuleName is UNIQUE
	//else {	
	//	sqlite3_command cmd(*conn, "SELECT filterRuleId FROM FilterRule WHERE FilterRuleName = ?;");
	//	cmd.bind(1, filter.name);
	//	sqlite3_cursor cursor = cmd.executecursor();
	//	if (cursor.step()) {
	//		// if there is already a filter with such name, set the
	//		// filter.id respectively
	//		filter.id = cursor.getint(0);
	//		cursor.close();
	//		return;
	//	}
	//  cursor.close();
	//}
	if (count > 0) {
		if (filter.id >= 0) {
			// it is already there -> update
			sqlite3_command cmd(*conn, "UPDATE FilterRule SET name = ?, rule = ? WHERE filterRuleId = ?;");
			cmd.bind(1, filter.name);
			cmd.bind(2, filter.rule);
			cmd.bind(3, filter.id);
			cmd.executenonquery();
		}
	} else {
		// it is not there -> insert
		sqlite3_command cmd(*conn, "INSERT INTO FilterRule (name,rule) VALUES (?,?);");
		cmd.bind(1, filter.name);
		cmd.bind(2, filter.rule);
		cmd.executenonquery();
		// get id which database automatically created
		filter.id = sqlite3_last_insert_rowid(conn->db());
	}
}

void FilterRulePersistence::load(FilterRule& filter, id_t id) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }

	filter = FilterRule(); // clean
	sqlite3_command cmd(*conn, "SELECT name,rule FROM FilterRule WHERE filterRuleId = ?;");
	cmd.bind(1, id);
	sqlite3_cursor cursor = cmd.executecursor();
	if (cursor.step()) {
		filter.id = id;
		filter.name = cursor.getstring(0);
		filter.rule = cursor.getstring(1);
	} else {
		cursor.close(); // a bit ugly: Duplicate close() call. Think how to do it better.
		throw new GetodoError("No such a filter rule to load.");
	}
	cursor.close();
}

void FilterRulePersistence::erase(id_t id) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	
	sqlite3_command cmd(*conn, "DELETE FROM FilterRule WHERE filterRuleId = ?;");
	cmd.bind(1, id);
	cmd.executenonquery();
}

void FilterRulePersistence::setName(FilterRule& filter, const std::string name) {
	filter.name = name;
	setColumn(filter.id, name, "name");
}

void FilterRulePersistence::setRule(FilterRule& filter, const std::string rule) {
	filter.rule = rule;
	setColumn(filter.id, rule, "rule");
}
void FilterRulePersistence::setColumn(id_t id, const std::string value, const std::string column) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	
	// TODO: check, if the filter rule really exists in database, else throw
	
	std::ostringstream ss;
	ss << "UPDATE FilterRule SET " << column << " = ? WHERE filterRuleId = ?;";
	sqlite3_command cmd(*conn, ss.str());
	cmd.bind(1, value);
	cmd.bind(2, id);
	cmd.executenonquery();
}

//FilterRule FilterBuilder::createAllTagsFilter(idset_t& tags) {
//	// TODO: find out tag names, put them into the name
//	// name: "all tags: tag1, tag2, tag3, ..."
//	// rule: "NATURAL JOIN Tagged WHERE tagId = 1 AND tagId = 2 AND tagId = 3"
//	
//	std::ostringstream ss;
//
//	join(std::cout, tags.begin(), tags.end(), ", ");
//	std::cout << std::endl;
//
//	std::vector<std::string> tagRules(tags.size());
//	int i = 0;
//	BOOST_FOREACH(id_t tagId, tags) {
//		tagRules[i] = std::string("tagId = "
//			+ boost::lexical_cast<std::string, id_t>(tagId));
//		std::cout << "DEBUG: tagRules[" << i << "] = "  << tagRules[i] << std::endl;
//		i++;
//	}
//	ss << "NATURAL JOIN Tagged WHERE ";
//	join(ss, tagRules.begin(), tagRules.end(), " AND ");
//	return FilterRule("tags", ss.str());
//}

FilterRule FilterBuilder::createTagFilter(id_t tagId) {
	std::string tagIdStr = boost::lexical_cast<std::string, id_t>(tagId);
	return FilterRule(
		"tag " + tagIdStr,
		"SELECT taskId FROM Tagged WHERE tagId = " + tagIdStr
	);
}

FilterRule FilterBuilder::unionFilters(const std::vector<FilterRule>& filters) {
	return joinFilters(filters, " UNION ", "INTERSECT");
}

FilterRule FilterBuilder::intersectFilters(const std::vector<FilterRule>& filters) {
	return joinFilters(filters, " INTERSECT ", "UNION");
}

FilterRule FilterBuilder::joinFilters(
		const std::vector<FilterRule>& filters,
		std::string command,
		std::string dualCommand
	) {
	bool simple = true;
	BOOST_FOREACH(FilterRule filter, filters) {
		if (filter.rule.find(dualCommand, 0) != std::string::npos) {
			simple = false;
			break;
		}
	}
	return joinFiltersImpl(filters, command, simple);
}

FilterRule FilterBuilder::joinFiltersImpl(
		const std::vector<FilterRule>& filters,
		std::string command,
		bool simple
	) {
	std::ostringstream ss;
	std::vector<std::string> filterStrings;
	BOOST_FOREACH(FilterRule filter, filters) {
		if (simple) {
			filterStrings.push_back(filter.rule);
		} else {
			filterStrings.push_back("SELECT * FROM (" + filter.rule + ")");
		}
	}
	join(ss, filterStrings.begin(), filterStrings.end(), command.c_str());
	return FilterRule("joined rule", ss.str());
}

} // namespace getodo
