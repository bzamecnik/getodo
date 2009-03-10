// $Id$
//
// C++ Interface: TaskManager
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
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
 * Storage and manipulation of tasks, tags and filter rules.
 */
class TaskManager : public sigc::trackable {
private:
    std::map<id_t,Task*> tasks;
    std::map<id_t,Tag*> tags;
    std::map<id_t,FilterRule*> filters;

    sqlite3_connection* conn;

	FilterRule activeFilterRule;
	idset_t visibleTasksCache; // task id's which have passed the filter
public:
    // TaskManager(); // for in-memory database (sqlite filename :memory:)
    TaskManager(std::string dbname);
    TaskManager(sqlite3_connection* conn);
    virtual ~TaskManager();

    // tip: a function for switching database connection
    // eg. when Save As is invoked (from one file to another, or from memory to file)
	// Maybe it will be needed to deep copy the whole database...
    
    // ----- SQLite connection -----
    
    sqlite3_connection* getConnection(); // really publish?
	//void setConnection(sqlite3_connection* conn);
    
    // ----- Task operations -----

    id_t addTask(const Task& task);
    bool hasTask(id_t taskId);
    Task* getTask(id_t taskId); // TODO: change return type to Task& and throw GetodoError
    TaskPersistence& getPersistentTask(id_t taskId); // for modyfing particular things
    Task& editTask(id_t taskId, const Task& task);
    void deleteTask(id_t taskId); //should throw an exception on failure
	std::vector<Task*>& getTasks();
	std::vector<Task*>& getTopLevelTasks();

    // ----- Tag operations -----

    id_t addTag(const Tag& tag);
    bool hasTag(id_t tagId);
    bool hasTag(std::string tagName);
    Tag& getTag(id_t tagId);
    Tag& getTag(std::string tagName);
    Tag& editTag(id_t tagId, const Tag& tag);
    void deleteTag(id_t tagId); //should throw an exception on failure
    std::vector<Tag*>& getTags();

    // ----- FilterRule operations -----

    id_t addFilterRule(const FilterRule& filter);
    bool hasFilterRule(id_t filterRuleId);
    bool hasFilterRule(std::string filterRuleName);
    FilterRule& getFilterRule(id_t filterRuleId);
    FilterRule& editFilterRule(id_t filterRuleId, const FilterRule& filter);
    void deleteFilterRule(id_t filterRuleId); //should throw an exception on failure
    std::vector<FilterRule*>& getFilterRules();

	void setActiveFilterRule(const FilterRule& filter); // enable filtering and set the rule
	void resetActiveFilterRule(); // deactivate filtering
	//FilterRule* getActiveFilterRule() const;
	bool hasActiveFilterRule() const;

	bool isTaskVisible(id_t taskId); // true, if taskId passed the filter
	idset_t getFilteredTasks();

	// ----- signals for models -----
	sigc::signal1<void, Task&> signal_task_inserted;
	sigc::signal1<void, Task&> signal_task_updated;
	sigc::signal1<void, Task&> signal_task_removed;
	sigc::signal2<void, Task&, id_t> signal_task_moved; // args: task being moved, old parent id

	sigc::signal1<void, Tag&> signal_tag_inserted;
	sigc::signal1<void, Tag&> signal_tag_updated;
	sigc::signal1<void, Tag&> signal_tag_removed;

	sigc::signal1<void, FilterRule&> signal_filter_inserted;
	sigc::signal1<void, FilterRule&> signal_filter_updated;
	sigc::signal1<void, FilterRule&> signal_filter_removed;
private:
    void loadAllFromDatabase(); // to be called by the constructor
    
    // void loadFromDatabase(const FilterRule& filter);
    // TODO: move functionality here and make loadAllFromDatabase()
    // call this function with empty filter
    
    void createEmptyDatabase(); // create an inital database structure
    bool checkDatabaseStructure(); // true, if all needed tables exist

	// TODO: Use output iterator and back_inserter.
	// See: http://bytes.com/groups/c/60522-returning-vector-reference#post225137
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

	// TODO:
    // - specify a format for FilterRules
    // - parse it, convert it to SQL query (WHERE)
    //idset_t filterTasks(id_t filterRuleId); // TODO
    idset_t& filterTasks(FilterRule& filterRule);
};

} // namespace getodo

#endif // LIBGETODO_TASKMANAGER_H
