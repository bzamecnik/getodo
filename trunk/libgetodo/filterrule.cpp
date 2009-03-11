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

id_t FilterRulePersistence::insert(FilterRule& filter) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }

	// id is defined NOT NULL, inserting NULL by not specifying
	// the value sets it to the ROWID (ie. it's auto-incremented)
	sqlite3_command cmd(*conn, "INSERT INTO FilterRule (name,rule) VALUES (?,?);");
	cmd.bind(1, filter.name);
	cmd.bind(2, filter.rule);
	cmd.executenonquery();
	// get id which database automatically created
	filter.id = sqlite3_last_insert_rowid(conn->db());
	return filter.id;
}

void FilterRulePersistence::update(const FilterRule& filter) {
	if (!conn) {
		throw new GetodoError("No database connection in the persistence.");
	}
	if (!filter.hasValidId()) {
		throw new std::invalid_argument("Invalid filter id: " + filter.id);
	}

	// UPDATE doesn't throw any error when given row doesn't exist
	sqlite3_command cmd(*conn, "UPDATE FilterRule SET name = ?, rule = ? WHERE filterRuleId = ?;");
	cmd.bind(1, filter.name);
	cmd.bind(2, filter.rule);
	cmd.bind(3, filter.id);
	cmd.executenonquery();
}

FilterRule FilterRulePersistence::load(id_t id) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }

	FilterRule filter = FilterRule(); // clean
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
	return filter;
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
	std::ostringstream ss;
	std::vector<std::string> filterStrings;
	BOOST_FOREACH(FilterRule filter, filters) {
		if (filter.isEmpty()) {
			continue;
		}
		if (filter.rule.find(dualCommand, 0) != std::string::npos) {
			filterStrings.push_back("SELECT * FROM (" + filter.rule + ")");
		} else {
			filterStrings.push_back(filter.rule);
		}
	}
	join(ss, filterStrings.begin(), filterStrings.end(), command.c_str());
	return FilterRule("joined rule", ss.str());
}

} // namespace getodo
