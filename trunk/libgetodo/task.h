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
 * Representation of a task. Object of class Task are data objects with
 * no reference to the database. TaskPersistence is used to communicate
 * with the database (insert, load, update, delete).
 */ 
class Task {
private:
	/** Id of the task. INVALID_ID, if not already in database */
	id_t taskId;
	/** Id of parent task. INVALID_ID, if the task has no parent task.
	 * Ie. it is a top level task.
	 */
	id_t parentId;
	/** Subtasks.
	 * Reverse relationship computed from parentId.
	 * Using sets of id_t here is better than Task& because of possible
	 * cycles in Subtask relation while loading from database.
	 */
	std::set<id_t> subtasks;

	/** Tags by which the task is tagged. */
	std::set<id_t> tags;

	/** Title of the task or short description. */
	std::string description;
	/** Longer description, possibly including links, attachments, etc. */
	std::string longDescription;

	// these dates might be variant in future, let's have them private
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

	/** Generic id of task which is not already in the database. */
	static const id_t INVALID_ID = -1;

	/** Check if task of such an id could be stored in the database. */
	static bool isValidId(id_t id);

	// ----- Access member functions -----

	/* Get id of the task. Could be INVALID_ID, if the task is not already in the database.*/ 
	id_t getTaskId() const;
	/* Set id of the task. */ 
	void setTaskId(id_t taskId);
	/** Check if task has such an id, so it should be stored in the database. */
	bool hasValidId() const;

	/** Get id of the parent task */
	id_t getParentId() const;
	//void setParentId(id_t parentId); // not needed
	
	/** Set parent task. A reference to TaskManager is given to tell the old
	 * and new parents of the change. Passing task manager is not a clean solution.
	 */
	void setParent(id_t newParentId, TaskManager& manager);
	/* Unset parent. Ie. make the task a top level one.
	 * A reference to TaskManager is given to tell the old
	 * and new parents of the change. Passing task manager is not a clean solution.
	 */
	void unsetParent(TaskManager& manager);
	/** Check if the task has a parent task, ie. it is not a top level task. */
	bool hasParent() const;

	std::string getDescription() const;
	void setDescription(const std::string& description);

	std::string getLongDescription() const;
	void setLongDescription(const std::string& longDescription);

	void addTag(id_t tagId); //should throw an exception on failure (?)
	bool hasTag(id_t tagId) const;
	void removeTag(id_t tagId); //should throw an exception on failure
	/** Get a list of tag ids which belong to the task. */
	idset_t& getTagIds() const;
	/** Get a list of tag names separated by a space. */
	std::string getTagsAsString(TaskManager& manager) const;
	/** Set tags from a string of tag names separated by whitespace. 
	 * Try to synchronize old and new tags to minimize database queries.
	*/
	void setTagsFromString(TaskManager& manager, const std::string& tagsString);

	// only update subtasks set. these two should be private!
	void addSubtask(id_t taskId); //should throw an exception on failure (?)
	void removeSubtask(id_t taskId); //should throw an exception on failure
	bool hasSubtask(id_t taskId) const;

	/** Get a list of subtask ids. */
	std::vector<id_t>& getSubtaskIds() const;

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
	/** Make a database row representing the task.
	 * All fields except subtasks and tags are serialized.
	 * Subtasks are only another view to parent relation.
	 * Tags need to be stored separately.
	 */
	databaseRow_t& toDatabaseRow() const;
	/** Create a task from a database row. Factory method. */
	static Task* fromDatabaseRow(databaseRow_t& row);

	// ----- text I/O -----
	/** String representation. For debugging purposes. For database
	 * serializaiton use toDatabaseRow().
	 */
	std::string toString() const;
	friend std::ostream& operator<< (std::ostream& o, const Task& task);
private:
	/** Convert set to vector. Utility function. */
	template<typename T>
	std::vector<T>& convertSetToVector(std::set<T> s) const {
		std::vector<T>& vector = *(new std::vector<T>());
		std::set<T>::const_iterator it;
		for (it = s.begin(); it != s.end(); it++) {
			vector.push_back(*it);
		}
		return vector;
	}
};

// ----- class TaskPersistence --------------------

/** %Task persistence.
 * Object-relation mapping of Task objects.
 * TODO:
 * Persistence should contain a reference to TaskManager,
 * not only to a database connection in order to call signals!
 */
class TaskPersistence {
private:
	/** Database connection. */
	sqlite3x::sqlite3_connection* conn;
	/** Task being persisted. */
	Task* task;
public:
	/** Constructor for loading new tasks from database. */
	TaskPersistence(sqlite3x::sqlite3_connection* conn);
	/** Constructor for modifying particular things in a task */
	TaskPersistence(sqlite3x::sqlite3_connection* conn, Task* task);
	~TaskPersistence();
	
	/** Save Task to database and assign an id.
	 * Return true, if successfully inserted.
	 */
	bool insert();
	/** Update an existing task (with valid taskId) */
	void update();

	/** Load Task from database. */
	Task* load(id_t taskId);

	/** Delete the task from database. */
	void erase();

	/** Get the task being persited. */
	Task* getTask() const;
	/** Set the task being persited. */
	void setTask(Task* task);

	// Wrappers of member functions from Task that modify the Task
	// - will call original function
	// - will make SQL query to reflect changes into database
	// Reason: We don't want to always save whole object, while making small changes.

	void setDescription(const std::string& description);
	void setLongDescription(const std::string& longDescription);

	void addTag(id_t tagId);
	void removeTag(id_t tagId);

	// TODO: set tags from string

	void setParentId();

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
	/** Update a single column and also last modified date. */
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

	/** Common code to insert() and update() */
	databaseRow_t& prepareRowToSave();
};

} // namespace getodo

#endif // LIBGETODO_TASK_H
