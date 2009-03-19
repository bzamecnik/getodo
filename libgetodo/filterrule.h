// $Id$
//
// Classes FilterRule, FilterRulePersistence, FilterBuilder
//
// Description: 
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
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
 * Rule for filtering tasks. Current implementation holds a literal SQL query
 * in the rule field. Eg.: "SELECT taskId FROM Task WHERE done = 0". Actual
 * filtering is provided by the filter() method.
 *
 * Filter rules can be persitently stored in a database. This is maintained by
 * FilterRulePersistence class. For an easy management each persisten rule has
 * an unique identification. To be more user friendly each rule can have a name.
 */
class FilterRule {
public:
	id_t id; //!< Rule identification
	std::string name; //!< Rule name
	std::string rule; //!< Actual filtering rule

	FilterRule();
	FilterRule(const FilterRule& r);
	FilterRule(std::string name, std::string rule);
	FilterRule(id_t id, std::string name, std::string rule);
	virtual ~FilterRule();

	/** Filter tasks using this rule.
	 * \param conn database connection
	 * \return set of task ids that passed the filter
	 */
	idset_t filter(boost::shared_ptr<sqlite3_connection> conn) const;

	/** Convert to string.
	 * \return a human-readable string representation of the filter rule
	 */
	std::string toString() const;
	friend std::ostream& operator<< (std::ostream& o, const FilterRule& rule);

	/** Invalid id.
	 * Filter rules persisted in database should not have this id.
	 */
	static const id_t INVALID_ID = -1;
	/** Check id validity.
	 * \return true, if id is valid
	 */
	static bool isValidId(id_t id);
	/** Check id validity of this rule.
	 * \return true, if its id is valid
	 */
	bool hasValidId() const;
	/** Check if filter rule is empty.
	 * \return true, if rule is empty
	 */
	bool isEmpty() const;
};

/** Filter rule persistence.
 * Object-relation mapping of filter rules.
 *
 * There is a CRUD (Create, Read, Update, Delete) interface:
 * - insert()
 * - load()
 * - update()
 * - erase()
 */
class FilterRulePersistence {
public:
	/** A constructor for loading new FilterRules
	 * \param conn A database connection. If it is not alright
	 * other methods will throw GetodoError exceptions.
	 */
	FilterRulePersistence(boost::shared_ptr<sqlite3_connection> conn);
	virtual ~FilterRulePersistence();

	/** Insert a new filter rule into the database.
	 * The rule is assumed to have no persistent id yet (eg. it was newly
	 * created), so a new id is assigned. It is both set to the given
	 * filter and returned.
	 *
	 * \throw GetodoError if the database connection is broken.
	 *
	 * \return id id assigned by the database.
	 */
	FilterRule insert(const FilterRule& filter);

	/** Update an existing filter rule.
	 * The rule is assumend to have a valid persistent id, otherwise
	 * an exception is thrown.
	 * If there's no such a rule in the database to update nothing happens.
	 *
	 * \throw GetodoError if the database connection is broken
	 * or if the id is not valid.
	 */
	void update(const FilterRule& filter);

	/** Load a filter rule from database.
	 * \throw GetodoError if the database connection is broken.
	 * \throw GetodoError if there's not such a rule in database.
	 *
	 * \param id id of a rule to load
	 * \return filter rule loaded
	 */
	FilterRule load(id_t id);
	
	/** Delete a filter rule from database.
	 * \throw GetodoError if the database connection is broken.
	 *
	 * \param id id of the rule to delete
	 */
	void erase(id_t id);

	/** Update name of the filter rule.
	 * \throw GetodoError if the database connection is broken.
	 *
	 * \param filter a filter rule to be modified
	 * \param name new filter rule name
	 */
	void setName(FilterRule& filter, const std::string name);
	/** Update actual rule of the filter rule.
	 * \throw GetodoError if the database connection is broken.
	 *
	 * \param filter a filter rule to be modified
	 * \param rule new rule contents
	 */
	void setRule(FilterRule& filter, const std::string rule);
private:
	boost::shared_ptr<sqlite3_connection> conn;
	/** Update particular field of a filter rule.
	 * This is a common implementation for setName() and setRule().
	 * \throw GetodoError if the database connection is broken.
	 *
	 * \param id id of the rule to update
	 * \param value new value
	 * \param column column name
	 */
	void setColumn(id_t id, const std::string value, const std::string column);
};

/** Filter Builder.
 * A class to support creating correct filtering rules via a set of functions.
 * It also supports joining filter rules using set union and intersection operators.
 */
class FilterBuilder {
public:
	/** Create a rule to filter by a tag.
	 * Create a filter rule to select tasks tagged with a given tag.
	 * 
	 * This is a factory method. The rule created has not a valid id
	 * (it has not been persisted).
	 *
	 * \param tagId tag id
	 * \return filter rule created
	 */
	static FilterRule createTagFilter(id_t tagId);
	
	/** Union filter rules.
	 * Create a filter rule to select tasks that pass _any_ of the rules given,
	 * ie. join the rules with OR operator.
	 *
	 * Implementation note: The SQL queries (SELECT statements) of each rule
	 * are joined using the UNION command. If a rule contains
	 * the INTERSECT command it is wrapped with a "SELECT * FROM (...)" to
	 * ensure correct operation.
	 *
	 * This is a factory method. The rule created has not a valid id
	 * (it has not been persisted).
	 * \param filters a list of existing filter rules
	 *
	 * \return filter rule created
	 */
	static FilterRule unionFilters(const std::vector<FilterRule>& filters);

	/** Intersect filter rules.
	 * Create a filter rule to select tasks that pass _all_ of the rules given,
	 * ie. join the rules with AND operator.
	 *
	 * Implementation note: The SQL queries (SELECT statements) of each rule
	 * are joined using the INTERSECT command. If a rule contains
	 * the UNION command it is wrapped with a "SELECT * FROM (...)" to
	 * ensure correct operation.
	 *
	 * This is a factory method. The rule created has not a valid id
	 * (it has not been persisted).
	 *
	 * \param filters a list of existing filter rules
	 * \return filter rule created
	 */
	static FilterRule intersectFilters(const std::vector<FilterRule>& filters);
private:
	/** Join filter rules.
	 * Create a filter rule from given rules join with a given command.
	 *
	 * If a rule contains the dualCommand it is wrapped with
	 * a "SELECT * FROM (...)".
	 *
	 * This is a factory method. The rule created has not a valid id
	 * (it has not been persisted).
	 *
	 * \param filters a list of existing filter rules
	 * \param command a command using which to join the rules
	 * \param dualCommand a command on which occurence to wrap the rule
	 * \return filter rule created
	 */
	// if there's any occurence of dualCommand in the filters,
	// we have to join them a more complicated way
	static FilterRule joinFilters(
		const std::vector<FilterRule>& filters,
		const std::string& command,
		const std::string& dualCommand
		);
};

} // namespace getodo

#endif // LIBGETODO_FILTERRULE_H
