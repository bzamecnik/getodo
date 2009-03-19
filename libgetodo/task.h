// $Id$
//
// Classes Task, TaskPersistence
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef LIBGETODO_TASK_H
#define LIBGETODO_TASK_H

#include "common.h"
#include "tag.h"
#include "taskmanager.h"
#include <sstream>

using namespace sqlite3x;

namespace getodo {

// ----- class Task --------------------

/** %Task.
 * Representation of a task. Instances of Task class are data objects with
 * no reference to the database. TaskPersistence is used to communicate
 * with the database (insert, load, update, delete).
 */ 
class Task {
private:
	/** Identification of the task.
	 * INVALID_ID if not already in database
	 */
	id_t taskId;
	/** Id of parent task.
	 * INVALID_ID if the task has no parent task. Ie. it is a top level task.
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
	/** Recurrence - how to repeat the task.
	 * Recurrence is an abstract base class, so there is a pointer.
	 */
	Recurrence* recurrence;
	
	int priority; // TODO: number or symbol?
	int completedPercentage;
	bool done;

public:
	/** Generic id of task which is not already in the database. */
	static const id_t INVALID_ID = -1;

	// ----- Constructors -----
	Task();
	Task(const Task& t); // copy constructor
	virtual ~Task();

	/** Make task's copy but clean it as it's not done yet.
	 * \return cleaned task copy
	 */
	boost::shared_ptr<Task> copyAsNew();

	// ----- Access member functions -----

	/** Check if task of such an id could be stored in the database. */
	static bool isValidId(id_t id);

	/* Get id of the task. Could be INVALID_ID, if the task is not already in the database.*/ 
	id_t getTaskId() const;
	/* Set id of the task. */ 
	void setTaskId(id_t taskId);
	/** Check if task has such an id, so it could be stored in database. */
	bool hasValidId() const;

	/** Get id of the parent task */
	id_t getParentId() const;
	//void setParentId(id_t parentId); // not needed
	
	/** Set parent task.
	 * A reference to TaskManager is given to tell the old and new parents
	 * of the change. However, passing task manager is not a clean solution.
	 */
	void setParent(id_t newParentId, TaskManager& manager);
	/* Unset parent.
	 * Make the task a top level one.
	 *
	 * A reference to TaskManager is given to tell the old and new parents
	 * of the change. However, passing task manager is not a clean solution.
	 */
	void unsetParent(TaskManager& manager);
	/** Check if the task has a parent task, so it is not a top level task. */
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
	 * \return database row with the task contents
	 */
	databaseRow_t& toDatabaseRow() const;
	/** Create a task from a database row.
	 * Factory method.
	 * \param row database row with the task contents
	 * \return newly created task
	 */
	static boost::shared_ptr<Task> fromDatabaseRow(databaseRow_t& row);

	// ----- text I/O -----
	/** String representation.
	 * This is useful for debugging purposes only.
	 * For database serializaiton use toDatabaseRow().
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
 *
 * The persistence has a reference to both the task to be modified
 * and he database connection.
 *
 * There is a CRUD (Create, Read, Update, Delete) interface:
 * - insert()
 * - load()
 * - update()
 * - erase()
 *
 * For modifying partiular things (ie. mak small changes) in the task without
 * a need to modify the whole there is a wrapped interface of Task class.
 * These function use setColumns().
 *
 * TODO:
 * Persistence should contain a reference to TaskManager,
 * not only to a database connection in order to call signals!
 */
class TaskPersistence {
private:
	/** Database connection. */
	boost::shared_ptr<sqlite3_connection> conn;
	/** Task being persisted. */
	boost::shared_ptr<Task> task;
public:
	/** Constructor for loading new tasks from database.
	 * \param conn A database connection. If it is not alright
	 * other methods will throw GetodoError exceptions.
	 */
	TaskPersistence(boost::shared_ptr<sqlite3_connection> conn);
	/** Constructor for modifying particular things in a task.
	 * Any changes made through TaskPersistence interface will
	 * show up in the \p task.
	 *
	 * \param conn A database connection. If it is not alright
	 * other methods will throw GetodoError exceptions.
	 * \param task task to modify
	 */
	TaskPersistence(boost::shared_ptr<sqlite3_connection> conn, boost::shared_ptr<Task> task);

	/** Copy constructor */
	TaskPersistence(const TaskPersistence& persistence);

	~TaskPersistence();
	
	/** Insert task to database and assign an id.
	 * \return true, if successfully inserted
	 */
	/** Insert a task tag into the database.
	 * The task is assumed to have no persistent id yet (eg. it was newly
	 * created), so a new id is assigned.
	 * 
	 * \throw GetodoError if the database connection is broken
	 *
	 \return true, if successfully inserted
	 */
	bool insert();
	
	/** Update an existing task (with valid taskId) */
	void update();

	/** Load task from database into Task object.
	 * Factory method. Load task, its tags and subtask relations and create
	 * a new Task instance.
	 * \throw GetodoError if the database connection is broken
	 * \throw GetodoError if there is no such a task
	 * \param taskId task id to load
	 */
	boost::shared_ptr<Task> load(id_t taskId);

	/** Delete the task from database. */
	void erase();

	/** Get the task being persited. */
	boost::shared_ptr<Task> getTask() const;

	///** Set the task being persited. */
	//void setTask(boost::shared_ptr<Task> task);

	// Wrappers of member functions from Task that modify the Task
	// - will call original function
	// - will make SQL query to reflect changes into database
	// Reason: We don't want to always save whole object, while making small changes.

	/** Set description.
	 * Do nothing when description doesn't change.
	 * \param description new task description
	 */
	void setDescription(const std::string& description);

	/** Set long description.
	 * Do nothing when long description doesn't change.
	 * \param longDescription new task long description
	 */
	void setLongDescription(const std::string& longDescription);

	/** Add a tag.
	 * Do nothing if the task already has this tag.
	 * \param tagId new tag id
	 */
	void addTag(id_t tagId);

	/** Remove a tag.
	 * Nothing happens if no such a tag exist.
	 * \param tagId id of tag to remove
	 */
	void removeTag(id_t tagId);

	// TODO: set tags from string

	/** Set parent task.
	 */
	void setParentId();

	//void addSubtask(id_t taskId); // deprecated
	//void removeSubtask(id_t taskId); // deprecated

	/** Set creation date. */
	void setDateCreated(const DateTime& dateCreated);

	/** Set last modification date. */
	void setDateLastModified(const DateTime& dateLastModified);

	/** Set start date. */
	void setDateStarted(const Date& dateStarted);
	
	/** Set deadline date. */
	void setDateDeadline(const Date& dateDeadline);

	/** Set completed date. */
	void setDateCompleted(const Date& dateCompleted);

	/** Set recurrence. */
	void setRecurrence(const Recurrence& r);

	/** Set priority. */
	void setPriority(int priority); // TODO: throw domain_error

	/** Set comleted percentage. */
	void setCompletedPercentage(int completedPercentage); // TODO: throw domain_error
	
	/** Check if task is done. */
	bool isDone();

	/** Set done. */
	void setDone(bool done = true);
private:
	/** Update a single column.
	 * Update last modification date too.
	 *
	 * \param columnName name of database column
	 * \param value new value
	 */
	template<typename T>
	void setColumn(std::string columnName, T value) {
		if (!conn) {
			throw GetodoError("No database connection in the persistence.");
		}
		if (!task || !task->hasValidId()) {
			throw GetodoError("Invalid task.");
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

	/** Common code to insert() and update() */
	databaseRow_t& prepareRowToSave();

	/** Check database connection.
	 * Check if the database connection is ok.
	 * \throw GetodoError if the database connection is broken
	 * \return true if ok
	 */
	bool checkDBConnection();

	/** Check task.
	 * Check if there is a task.
	 * \throw GetodoError if there is no such a task
	 * \return true if ok
	 */
	bool checkTask();

	/** Check if the task is persistent.
	 * Check if the task has been already stored in the database.
	 * \throw GetodoError if the task is not persisted
	 * \throw GetodoError if there is no such a task
	 * \return true if ok
	 */
	bool checkTaskPersistent();
};

} // namespace getodo

#endif // LIBGETODO_TASK_H
