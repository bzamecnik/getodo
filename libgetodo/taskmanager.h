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

class TaskManager : public sigc::trackable {
private:
    std::map<id_t,Task*> tasks;
    std::map<id_t,Tag*> tags;
    std::map<id_t,FilterRule*> filters;

    sqlite3_connection* conn;
public:
    // TaskManager(); // for in-memory database (sqlite filename :memory:)
    TaskManager(std::string dbname);
    TaskManager(sqlite3_connection* conn);
    virtual ~TaskManager();

    // tip: a function for switching database connection
    // eg. when Save As is invoked (from one file to another, or from memory to file)
    
    // ----- SQLite connection -----
    
    sqlite3_connection* getConnection(); // really publish?
    
    // ----- Task operations -----

    Task* addTask(Task* task);
    bool hasTask(id_t taskId);
    Task* getTask(id_t taskId);
    TaskPersistence& getPersistentTask(id_t taskId); // for modyfing particular things
    Task* editTask(id_t taskId, const Task& task);
    Task* editTask(Task* task);
    void deleteTask(id_t taskId); //should throw an exception on failure

    // ----- Tag operations -----

    void addTag(const Tag& tag);
    bool hasTag(id_t tagId);
    // bool hasTag(std::string tagName); // TODO
    Tag& getTag(id_t tagId);
    // Tag& getTag(std::string tagName); // TODO
    Tag& editTag(id_t tagId, const Tag& tag);
    void deleteTag(id_t tagId); //should throw an exception on failure
    std::list<Tag*> getTagsList();

    // ----- FilterRule operations -----

    void addFilterRule(FilterRule& filter);
    bool hasFilterRule(id_t filterRuleId);
    // bool hasFilterRule(std::string filterRuleName); // TODO
    FilterRule& getFilterRule(id_t filterRuleId);
    FilterRule& editFilterRule(id_t filterRuleId, const FilterRule& filter);
    void deleteFilterRule(id_t filterRuleId); //should throw an exception on failure
    std::list<FilterRule> getFilterRulesList() const;

    // TODO:
    // - specify a format for FilterRules
    // - parse it, convert it to SQL query (WHERE)
    taskset_t filterTasks(id_t filterRuleId); // TODO
    taskset_t filterTasks(const FilterRule& filterRule); // TODO

	// ----- signals for models -----
	sigc::signal1<void, Tag&> signal_tag_inserted;
	sigc::signal1<void, Tag&> signal_tag_updated;
	sigc::signal1<void, Tag&> signal_tag_removed;
private:
    void loadAllFromDatabase(); // to be called by the constructor
    
    // void loadFromDatabase(const FilterRule& filter);
    // TODO: move functionality here and make loadAllFromDatabase()
    // call this function with empty filter
    
    void createEmptyDatabase(); // create an inital database structure
    bool checkDatabaseStructure(); // true, if all needed tables exist

    template<typename T_key, typename T_value>
    std::list<T_value> convertMapToList(std::map<T_key, T_value *> m) const {
        std::list<T_value> list;
        std::map<T_key, T_value *>::const_iterator it;
        for (it = m.begin(); it != m.end(); it++) {
            list.push_front(*(it->second));
        }
        return list;
    }
};

} // namespace getodo

#endif // LIBGETODO_TASKMANAGER_H
