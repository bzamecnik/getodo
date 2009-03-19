// $Id$
//
// Class TaskManager
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef LIBGETODO_TASKMANAGER_H
#define LIBGETODO_TASKMANAGER_H

#include "common.h"
#include "tag.h"
#include "filterrule.h"
#include "task.h"
#include <sigc++/sigc++.h>
#include <set>

namespace getodo {

using namespace sqlite3x;

/** %Task manager.
 * Storage and manipulation of tasks, tags and filter rules. It act as
 * an object representation of what is in the database with some functionality
 * added on top of it. Object stored here are persisted in the database.
 *
 * %Task manager provides a way to filter tasks.
 *
 * There are some signals available. They are useful for example to let
 * a TreeView model know when a task changes or is inserted.
 */
class TaskManager : public sigc::trackable {
private:
    std::map<id_t,Task*> tasks;
    std::map<id_t,Tag> tags;
    std::map<id_t,FilterRule> filters;

	boost::shared_ptr<sqlite3_connection> conn;

	/** Active filter rule. */
	FilterRule activeFilterRule;

	/**  Visible tasks cache.
	 * Task id's which have passed the active filter.
	 */
	idset_t visibleTasksCache;
public:
    // TaskManager(); // for in-memory database (sqlite filename :memory:)
    
	/** %Task manager constructor.
	 * Create a task manager and load it from the database.
	 * If the database is not initialized, create the table structure.
	 * \param dbname filename of the database
	 */
	TaskManager(const std::string& dbname);

	/** %Task manager constructor.
	 * Create a task manager and load it from the database using a previously
	 * opened database connection.
	 * If the database is not initialized, create the table structure.
	 * \param conn database connection
	 */
    TaskManager(boost::shared_ptr<sqlite3_connection> conn);

    virtual ~TaskManager();

    // tip: a function for switching database connection
    // eg. when Save As is invoked (from one file to another, or from memory to file)
	// Maybe it will be needed to deep copy the whole database...
    
    // ----- SQLite connection -----
    
	/** Get the database connection.
	 * \return SQLite connection
	 */
    boost::shared_ptr<sqlite3_connection> getConnection(); // really publish?

	//void setConnection(boost::shared_ptr<sqlite3_connection> conn);
    
    // ----- Task operations -----

	/** Add a task.
	 * Add a new task and save it to database.
	 * 
	 * Raise #signal_task_inserted.
	 *
	 * \param task task to add
	 * \return id assigned by the database
	 */
    id_t addTask(const Task& task);

	/** Check if a task is already in task manager.
	 * \param taskId task identification
	 * \return true if present
	 */
    bool hasTask(id_t taskId);

	/** Get a task from task manager.
	 * TODO: change the return type to Task& or shared_ptr<Task>.
	 *
	 * \param taskId task identification
	 * \return pointer to task or 0 if not found
	 */
    Task* getTask(id_t taskId);

	/** Get task persistence for the task.
	 * This is useful for modifying particular things without the need
	 * to update the whole task.
	 *
	 * \param taskId task identification
	 * \return task persistence
	 */
    TaskPersistence& getPersistentTask(id_t taskId);
	/** Edit task.
	 * Replace contents of task identified by \p taskId by contents
	 * of \p task.
	 *
	 * Raise #signal_task_updated.
	 *
	 * Tags are synchronized in TaskPersistence::update().
	 * 
	 * \throw GetodoError if a task with such a taskId is not present
	 * 
	 * \param taskId existing task identification
	 * \param task new task contents
	 * \return updated task
	 */
    Task& editTask(id_t taskId, const Task& task);

	/** Delete task and its whole subtree.
	 * Delete the task and recursively the whole tree of its subtasks.
	 * Also remove tagging relations of all tasks being deleted. However,
	 * tags themselves remain.
	 * 
	 * Raise #signal_task_removed.
	 *
	 * \throw GetodoError if there's no such a task in the task manager.
	 *
	 * \param taskId task identification
	 */
    void deleteTask(id_t taskId);

	/** Get all tasks present in task manager.
	 * This is useful for batch operations with all the tasks.
	 * \return vector of all tasks
	 */
	std::vector<Task*>& getTasks();

	/** Get all top-level tasks present in task manager.
	 * Top level tasks are those which have no parent. This is useful for batch
	 * operations with all the tasks.
	 * \return vector of all top-level tasks
	 */
	std::vector<Task*>& getTopLevelTasks();

    // ----- Tag operations -----

	/** Add a tag.
	 * Add a new tag and save it to database.
	 * 
	 * Raise #signal_tag_inserted.
	 *
	 * \param tag tag to add
	 * \return id assigned by the database or Tag::INVALID_ID on error
	 */
    id_t addTag(const Tag& tag);

	/** Check presence of tag by its id.
	 * \param tagId tag identification
	 * \return true if such a tag is present in task manager
	 */
    bool hasTag(id_t tagId);

	/** Check presence of tag by its name.
	 * \param tagName tag name
	 * \return true if such a tag is present in task manager
	 */
    bool hasTag(const std::string& tagName);

	/** Get tag by its id.
	 * \throw GetodoError if the tag is not found.
	 *
	 * \param tagId tag identification
	 * \return reference to the tag if it's is present in task manager
	 */
    Tag getTag(id_t tagId);

	/** Get tag by its name.
	 * \throw GetodoError if the tag is not found.
	 *
	 * \param tagName tag name
	 * \return reference to the tag if it's is present in task manager
	 */
    Tag getTag(const std::string& tagName);

	/** Edit tag.
	 * Replace contents of tag identified by \p tagId by contents of \p tag.
	 * 
	 * Raise #signal_tag_updated.
	 *
	 * \throw GetodoError if the tag is not found.
	 *
	 * \param tagId existing tag id
	 * \param tag new tag
	 * \return reference to the updated tag
	 */
    Tag editTag(id_t tagId, const Tag& tag);

	/** Delete tag.
	 * Raise #signal_tag_removed.
	 *
	 * \throw GetodoError if there is not such a tag in the task manager.
	 *
	 * \param tagId tag identification
	 */
    void deleteTag(id_t tagId);

	/** Get all tags present in task manager.
	 * This is useful for batch operations with all the tags.
	 * \return vector of all tags
	 */
    std::vector<Tag> getTags();

    // ----- FilterRule operations -----

	/** Add a filter rule.
	 * Add a new filter rule and save it to database. Id is automatically
	 * assigned by the database.
	 * 
	 * Raise #signal_filter_inserted.
	 *
	 * \param filter filter rule to add
	 * \return id assigned by the database
	 */
    id_t addFilterRule(const FilterRule& filter);

	/** Check presence of filter rule by its id.
	 * \param filterRuleId filter rule identification
	 * \return true if such a filter rule is present in task manager
	 */
    bool hasFilterRule(id_t filterRuleId);

	/** Check presence of filter rule by its name.
	 * \param filterRuleName filter rule name
	 * \return true if such a filter rule is present in task manager
	 */
    bool hasFilterRule(const std::string& filterRuleName);

	/** Get filter rule by its id.
	 * \throw GetodoError if the filter rule is not found.
	 *
	 * \param filterRuleId filter rule identification
	 * \return reference to the filter rule if it's is present in task manager
	 */
    FilterRule getFilterRule(id_t filterRuleId);

	/** Edit filter rule.
	 * Replace contents of filter rule identified by \p filterRuleId by
	 * contents of \p filter.
	 * 
	 * Raise #signal_filter_updated.
	 *
	 * \throw GetodoError if the filter rule is not found.
	 *
	 * \param filterRuleId existing filter rule identification
	 * \param filter new filter rule
	 * \return reference to the updated tag
	 */
    FilterRule editFilterRule(id_t filterRuleId, const FilterRule& filter);

	/** Delete filter rule.
	 * Raise #signal_filter_removed.
	 *
	 * \throw GetodoError if there is not such a filter rule in the task manager.
	 *
	 * \param filterRuleId filter rule identification
	 */
    void deleteFilterRule(id_t filterRuleId);

	/** Get all filter rules present in task manager.
	 * This is useful for batch operations with all the filter rules.
	 * \return vector of all filter rules
	 */
    std::vector<FilterRule> getFilterRules();

	/** Filter tasks.
	 * Filter tasks in task manager using a given filter rule.
	 *
	 * \param filterRule filter rule
	 * \return set of task id that passed the filter, ie. they are visible
	 */
    idset_t filterTasks(const FilterRule& filterRule) const;

	/** Set the active filtering rule.
	 * Set \p filter as the active rule, filter tasks using this rule and
	 * store them in visibleTasksCache for getting via getFilteredTasks().
	 * If the rule is broken unset the rule and clear the visibleTasksCache.
	 *
	 * \param filter filter to set active
	 */
	void setActiveFilterRule(const FilterRule& filter);

	/** Unset the active filtering rule.
	 * And clear the visibleTasksCache.
	 */
	void resetActiveFilterRule();

	//FilterRule* getActiveFilterRule() const;

	/** Check if any active filter rule is present.
	 * \return true if there is any active filter rule
	 */
	bool hasActiveFilterRule() const;

	/** Check task visibility under active filter rule.
	 * Note: id's of task that passed the filter are visibleTaskCache.
	 * 
	 * \param taskId identification of task to check
	 * \return true if the task is passed the filter
	 */
	bool isTaskVisible(id_t taskId) const;

	/** Get id's of task that passed the filter.
	 * \return set of task id's that passed the filter, eg. they are visible
	 */
	idset_t getFilteredTasks() const;

	// ----- signals for models -----
	sigc::signal1<void, Task&> signal_task_inserted;
	sigc::signal1<void, Task&> signal_task_updated;
	sigc::signal1<void, Task&> signal_task_removed;
	
	/** Signal that task has moved.
	 * Task has moved if it changed its parent task.
	 *
	 * Arguments:
	 * - task being moved
	 * - old parent id
	 */
	sigc::signal2<void, Task&, id_t> signal_task_moved;

	sigc::signal1<void, Tag&> signal_tag_inserted;
	sigc::signal1<void, Tag&> signal_tag_updated;
	sigc::signal1<void, Tag&> signal_tag_removed;

	sigc::signal1<void, FilterRule&> signal_filter_inserted;
	sigc::signal1<void, FilterRule&> signal_filter_updated;
	sigc::signal1<void, FilterRule&> signal_filter_removed;
protected:
	/** Try to load things from database.
	 * If the database is not initialized create the table structure.
	 */
	void tryLoadingFromDatabase();

	/** Load everything from database in a batch.
	 * This is usually called by the TaskManager constructor.
	 */
    void loadAllFromDatabase();
    
	/** Initialize the database.
	 * Create the initial table structure.
	 */
    void createEmptyDatabase();

	/** Check database structure.
	 * \return true if all the needed tables exist
	 */
    bool checkDatabaseStructure();

	/** Convert map to vector.
	 * Make a vector from map values where value is a pointer.
	 * 
	 * TODO: Use output iterator and back_inserter.
	 * See: http://bytes.com/groups/c/60522-returning-vector-reference#post225137 
	 *
	 * \param m source map
	 * \return vector of map values
	 */
    template<typename T_key, typename T_value>
    std::vector<T_value*>& convertMapToVector(std::map<T_key, T_value *>& m)
	{
        std::vector<T_value*>& vector = *(new std::vector<T_value*>());
        std::map<T_key, T_value *>::iterator it;
        for (it = m.begin(); it != m.end(); it++) {
            vector.push_back(it->second);
        }
        return vector;
    }
};

} // namespace getodo

#endif // LIBGETODO_TASKMANAGER_H
