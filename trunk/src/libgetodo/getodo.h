// $Id: getodo.h 11 2008-06-25 13:20:57Z bohumir.zamecnik $

#include <list>
#include <map>
#include <set>
#include <string>

#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "sqlite3x/sqlite3x.hpp"

namespace getodo {

typedef int id_t;
typedef std::map<std::string,std::string> databaseRow_t;

class Tag {
	id_t tagId;
	std::string tagName;
public:	
	Tag();
	Tag(std::string tagName);
	~Tag();
	
	std::string getTagName();
	Tag& setTagName(std::string tagName);
	id_t getTagId();
};

// THINK: make AbstractDate a common ancestor to Date and DateTime?

class DateTime {
public:
	boost::posix_time::ptime date;
	
	DateTime() {
		date = boost::posix_time::ptime(boost::date_time::not_a_date_time);
	}
	DateTime(boost::posix_time::ptime const& date) {
		this->date = date;
	}
	DateTime(DateTime const& date) {
		this->date = date.date;
	}
	
	// For database storage.
	// Format: YYYY-MM-DD HH:MM:SS
	DateTime fromString(std::string str);
	std::string toString(DateTime const& date);
};

class Date {
public:
	boost::gregorian::date date;
	
	Date() : date(boost::gregorian::date(boost::date_time::not_a_date_time)) {}
	Date(boost::gregorian::date const& d) :	date(d) {}
	Date(Date const& d) : date(d.date) {}
	
	// For database storage.
	// Format: YYYY-MM-DD
	static Date fromString(std::string str);
	static std::string toString(Date const& date);
	std::string toString();
};

//class FuzzyDate {
	/*
	Date should be able to express:
		* date + time exactly given
		* date alone
		* indefinite date
		* no information
	*/
//}

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
	virtual Date next()=0;
	virtual Recurrence& fromString(std::string str)=0;
	virtual std::string toString(Recurrence const& r)=0;
	virtual std::string toString()=0;
	virtual ~Recurrence();
};

class RecurrenceOnce : public Recurrence {};
class RecurrenceDaily : public Recurrence {};
class RecurrenceWeekly : public Recurrence {};
class RecurrenceMonthly : public Recurrence {};
class RecurrenceYearly : public Recurrence {};
class RecurrenceIntervalDays : public Recurrence {};

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
};

class FilterRule {
	id_t filterRuleId;
	std::string name;
	std::string rule;
};

class Task {
	id_t taskId;
	std::string description;
	std::string longDescription; // including links, attachments, etc.

	// what to use: sets of id_t or Task&?
	std::set<id_t> tags;
	std::set<id_t> subtasks;

	DateTime dateCreated;
	DateTime dateLastModified;
	Date dateStarted;
	Date dateDeadline; // should be FuzzyDate
	Date dateCompleted;
//	Duration& estDuration; // estimated duration
//	Recurrence& recurrence;
	
	int priority; // TODO: number or symbol?
	int completedPercentage;

public:
	Task();
	Task(Task const& t); // copy constructor
	virtual ~Task(); // delete tags, subtasks
	
	// setters and getters for some properties
	id_t getTaskId();
	
	// convert representation: database <-> object
	static std::map<std::string,std::string> toDatabaseRow(Task& task);
	std::map<std::string,std::string> toDatabaseRow();
	static Task& fromDatabaseRow();
};

class TaskManager {
	std::map<id_t,Task*> tasks;
	std::map<id_t,Tag*> tags;
	std::list<FilterRule> filters;

	sqlite3x::sqlite3_connection* db;

public:
	TaskManager(std::string const &dbname);
	~TaskManager();

	Task& addTask(Task* task);
	Task& getTask(id_t taskId);
	Task& editTask(id_t taskId, Task& task);
	bool deleteTask(id_t taskId);

	Tag& addTag(Tag& tag);
	Tag& editTag(id_t tagId, Tag& tag);
	bool deleteTag(id_t tagId);

	bool assignTag(id_t tagId, id_t taskId);
	bool removeTag(id_t tagId, id_t taskId);

	FilterRule& addFilterRule(FilterRule& filter);
	FilterRule& editFilterRule(id_t filterRuleId, FilterRule& filter);
	bool deleteFilterRule(id_t filterRuleId);

	std::map<id_t,Task*> filterTask(id_t filterRuleId);
	std::map<id_t,Task*> filterTask(FilterRule& filter);

private:
	void loadFromDatabase();
	void loadFromDatabase(FilterRule& filter);
};

} // namespace getodo
