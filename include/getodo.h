// $Id$

#include <list>
#include <map>
#include <string>

#include "boost/date_time/gregorian/gregorian.hpp"

typedef int id_t;
typedef std::map<string,string> databaseRow_t;

class Task {
	id_t taskId;
	string description;
	string longDescription; // including links, attachments, etc.

	std::list<Tag&> tags;
	std::list<Task&> subtasks;

	DateTime dateCreated;
	DateTime dateLastModified;
	Date dateStarted;
	FuzzyDate dateDeadline;
	Date dateCompleted;
	Duration& estDuration; // estimated duration
	Recurrence& recurrence;
	
	int priority; // TODO: number or symbol?
	int completedPercentage;

public:
	Task();
	//{
	//	taskId = 0; // not yet set
	//}
	Task(Task& t); // copy constructor
	virtual ~Task(); // delete tags, subtasks
	
	// setters and getters for some properties

	// convert representation: database <-> object
	static std::map<string,string> toDatabaseRow(Task& task);
	std::map<string,string> toDatabaseRow();
	static Task& fromDatabaseRow();
}

class Tag {
	id_t tagId;
	string tagName;

public:	
	Tag();
	Tag(string tagName);
	~Tag();
	
	string getTagName();
	Tag& setTagName(string tagName);
	id_t getTagId();
}

class DateTime {
	// Date should be able to express: date + time exactly given
	boost::posix_time::ptime date;
}		
class Date {
	// date alone or no information
	boost::gregorian::date date;		
}

class FuzzyDate {
	/*
	Date should be able to express:
		* date + time exactly given
		* date alone
		* indefinite date
		* no information			
	*/
}

class Recurrence {
	/*
	Recurrence should be:
		*  once - no recurrence - default
		*  every Nth day
		*  every Nth week - given selected weekdays
		*  every Nth month - given day in month
		*  every Nth year - given day and month
		*  every day in an interval between two days
		Use: boost::gregorian::date_iterator				 		
	*/
}

class Duration {
	/*
		Duration should express:
			* units of given magnitude (minutes, hours, days, weeks, months, years)
			* - lesser magnitude => greater precision			
			* or no information
		Use:
			boost::gregorian::years, boost::gregorian::months, boost::gregorian::weeks
			boost::gregorian::date_duration 
			boost::posix_time::hours, boost::posix_time::minutes, boost::posix_time::seconds								
	*/
}

class FilterRule {
	id_t filterRuleId;
	string name;
	string rule;
}

class DatabaseConnection {
	// connection to a SQLite file
	//sqlite3 db;
}

class TaskManager {
	std::map<id_t,Task&> tasks;
	std::map<id_t,Tag&> tags;
	std::list<FilterRule&> filters;

	DatabaseConnection& db;

public:
	//TaskManager();
	TaskManager(DatabaseConnection& db);
	//TaskManager(TaskManager& tm);
	~TaskManager();

	Task& addTask(Task& task);
	Task& getTask(id_t taskId);
	Task& editTask(id_t taskId, Task& task);
	bool deleteTask(id_t taskId);

	Tag& addTag(Tag& tag);
	Tag& editTag(id_t tagId, Tag& tag);
	bool deleteTag(id_t tagId);

	assignTag(id_t tagId, id_t taskId);
	removeTag(id_t tagId, id_t taskId);

	FilterRule& addFilterRule(FilterRule& filter);
	FilterRule& editFilterRule(id_t filterRuleId, FilterRule& filter);
	bool deleteFilterRule(id_t filterRuleId);

	std::map<id_t,Task> filterTask(id_t filterRuleId);
	std::map<id_t,Task> filterTask(FilterRule& filter);

	private loadFromDatabase();
	loadFromDatabase(FilterRule& filter);
}
