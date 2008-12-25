// $Id$
//
// C++ Interface: Task
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef LIBGETODO_TASK_H
#define LIBGETODO_TASK_H

#include "common.h"
#include "tag.h"
#include "taskmanager.h"
#include <sstream>

namespace getodo {

// ----- class Task --------------------

/** %Task.
 * Representation of a task.
 */ 
class Task {
private:
	id_t taskId; // -1, if not already in database
	id_t parentId; // -1, if the task has no parent task
	// Reverse relationship computed from parentId.
	// Using sets of id_t here is better than Task& because of possible
	// cycles in Subtask relation while loading from database.
	std::set<id_t> subtasks;

	std::set<id_t> tags;

	std::string description;
	std::string longDescription; // including links, attachments, etc.

	// these dates might be variant in future, let's have it private
	DateTime dateCreated;
	DateTime dateLastModified;
	Date dateStarted;
	Date dateDeadline; // should be FuzzyDate
	Date dateCompleted;

	//Duration& estDuration; // estimated duration
	Recurrence* recurrence; // using abstract base class -> pointer
	
	int priority; // TODO: number or symbol?
	int completedPercentage;
	bool done;

public:
	// ----- Constructors -----
	Task();
	Task(const Task& t); // copy constructor
	virtual ~Task();

	static const id_t INVALID_ID = -1;

	static bool isValidId(id_t id);

	// ----- Access member functions -----

	id_t getTaskId() const;
	void setTaskId(id_t taskId);
	bool hasValidId() const;

	id_t getParentId() const;
	//void setParentId(id_t parentId); // not needed
	void setParent(Task& newParent);
	void unsetParent(); // make this task a root level task
	bool hasParent() const; // false if root

	std::string getDescription() const;
	void setDescription(const std::string& description);

	std::string getLongDescription() const;
	void setLongDescription(const std::string& longDescription);

	void addTag(id_t tagId); //should throw an exception on failure (?)
	bool hasTag(id_t tagId) const;
	void removeTag(id_t tagId); //should throw an exception on failure
	std::list<id_t> getTagsList() const;
	std::string getTagsAsString(TaskManager& manager) const;
	void setTagsFromString(TaskManager& manager, const std::string& tagsString);

	// update both this task and subtask being operated
	void addSubtask(Task& subtask);
	void removeSubtask(Task& subtask);
	// only update subtasks set. these two should be private!
	void addSubtask(id_t taskId); //should throw an exception on failure (?)
	void removeSubtask(id_t taskId); //should throw an exception on failure
	bool hasSubtask(id_t taskId) const;

	std::list<id_t> getSubtasksList() const;

	DateTime getDateCreated() const;
	void setDateCreated(const DateTime& dateCreated);

	DateTime getDateLastModified() const;
	void setDateLastModified(const DateTime& dateLastModified);

	Date getDateStarted() const;
	void setDateStarted(const Date& dateStarted);

	Date getDateDeadline() const; // should be FuzzyDate
	void setDateDeadline(const Date& dateDeadline);

	Date getDateCompleted() const;
	void setDateCompleted(const Date& dateCompleted);

	Recurrence& getRecurrence() const;
	void setRecurrence(Recurrence* r);

	int getPriority() const;
	void setPriority(int priority);

	int getCompletedPercentage() const;
	void setCompletedPercentage(int completedPercentage);
	
	bool isDone();
	void setDone(bool done = true);

	// ----- object-relation representation conversion -----
	databaseRow_t& toDatabaseRow() const;
	static Task* fromDatabaseRow(databaseRow_t& row);

	// ----- text I/O -----
	std::string toString() const;
	friend std::ostream& operator<< (std::ostream& o, const Task& task);
private:
	template<typename T>
	std::list<T> convertSetToList(std::set<T> s) const {
		std::list<T> list;
		std::set<T>::const_iterator it;
		for (it = s.begin(); it != s.end(); it++) {
			list.push_front(*it);
		}
		return list;
	}
};

// ----- class TaskPersistence --------------------

/** %Task persistence.
 * Object-relation mapping of Task objects.
 */
// TODO:
// Persistence should contain a reference to TaskManager,
// not only to a database connection in order to call signals!
class TaskPersistence {
private:
	sqlite3x::sqlite3_connection* conn;
	Task* task;
public:
	// Constructor for loading new Tasks
	TaskPersistence(sqlite3x::sqlite3_connection* conn);
	// Constructor for modifying particular things in a Task
	TaskPersistence(sqlite3x::sqlite3_connection* conn, Task* task);
	~TaskPersistence();
	
	// save Task to database and assign an id
	// true, if really inserted
	bool insert();
	// update existing Task (with valid taskId)
	void update();

	// load Task from database
	Task* load(id_t taskId);

	void erase();

	Task* getTask() const;
	void setTask(Task* task);

	// Wrappers of member functions from Task that modify the Task
	// - will call original function
	// - will make SQL query to reflect changes into database
	// Reason: We don't want to always save whole object, while making small changes.

	void setDescription(const std::string& description);
	void setLongDescription(const std::string& longDescription);

	void addTag(id_t tagId);
	void removeTag(id_t tagId);

	void setParentId(id_t);

	//void addSubtask(id_t taskId); // deprecated
	//void removeSubtask(id_t taskId); // deprecated

	void setDateCreated(const DateTime& dateCreated);
	void setDateLastModified(const DateTime& dateLastModified);
	void setDateStarted(const Date& dateStarted);
	void setDateDeadline(const Date& dateDeadline);
	void setDateCompleted(const Date& dateCompleted);

	void setRecurrence(const Recurrence& r);

	void setPriority(int priority); // TODO: throw domain_error

	void setCompletedPercentage(int completedPercentage); // TODO: throw domain_error
	
	bool isDone();
	void setDone(bool done = true);
private:
	// update a single column and also last modified date
	template<typename T>
	void setColumn(std::string columnName, T value) {
		if (!conn) {
			throw new GetodoError("No database connection in the persistence.");
		}
		if (!task || !task->hasValidId()) {
			throw new GetodoError("Invalid task.");
		}
		DateTime modifiedDate = DateTime::now();
		task->setDateLastModified(modifiedDate);
		sqlite3_command cmd(*conn);
		std::ostringstream ss;
		ss << "UPDATE Task SET " << columnName << " = ?, "
			"dateLastModified = ? WHERE taskId = ?;";
		cmd.prepare(ss.str());
		cmd.bind(1, value);
		cmd.bind(2, modifiedDate.toString());
		cmd.bind(3, task->getTaskId());
		cmd.executenonquery();
	}

	void saveTags();

	// common code to insert() and update()
	databaseRow_t& prepareRowToSave();
};

} // namespace getodo

#endif // LIBGETODO_TASK_H
