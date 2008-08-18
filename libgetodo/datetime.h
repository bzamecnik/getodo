// $Id:$
//
// C++ Interface: DateTime, Date
//
// Description: classes DateTime, Date, Recurrence*, Duration
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_DATETIME_H
#define LIBGETODO_DATETIME_H

#include <string>

#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>

namespace getodo {

// THINK: make AbstractDate a common ancestor to Date and DateTime?

class DateTime {
public:
	boost::posix_time::ptime date;
	
	DateTime();
	DateTime(const boost::posix_time::ptime& d);
	DateTime(const DateTime& d); // copy
	explicit DateTime(std::string d);
	// explicit DateTime(Date d);
	
	static DateTime now();

	static std::string toString(const DateTime& date);
	std::string toString() const;
private:
	// format for string representation
	// eg. YYYY-MM-DD HH:MM:SS
	static std::string format;
};

class Date {
public:
	boost::gregorian::date date;
	
	Date();
	Date(const boost::gregorian::date& d);
	Date(const Date& d);
	explicit Date(std::string d);

	static Date now();
	
	// Format: YYYY-MM-DD
	static std::string toString(const Date& date);
	std::string toString() const;

	static std::string format;
};

//class FuzzyDate {
	/* TODO
	Date should be able to express:
		* date + time exactly given
		* date alone
		* indefinite date
		* no information
	*/
//}

class Recurrence {
	/* TODO
	Recurrence should be:
		*  once - no recurrence - default
		*  every Nth day
		*  every Nth week - given selected weekdays
		*  every Nth month - given day in month
		*  every Nth year - given day and month
		*  every day in an interval between two days
		Use: boost::gregorian::date_iterator
	*/
public:
	virtual ~Recurrence();

	virtual Date next()=0;
	virtual std::string toString()const =0;
	
	static Recurrence* fromString(std::string str);
};

class RecurrenceOnce : public Recurrence {
public:
	RecurrenceOnce();
	virtual ~RecurrenceOnce();
	virtual Date next();
	virtual std::string toString() const;
};
class RecurrenceDaily : public Recurrence {
public:
	explicit RecurrenceDaily(std::string s);
	virtual ~RecurrenceDaily();
	virtual Date next();
	virtual std::string toString() const;
};
class RecurrenceWeekly : public Recurrence {
public:
	explicit RecurrenceWeekly(std::string s);
	virtual ~RecurrenceWeekly();
	virtual Date next();
	virtual std::string toString() const;
};
class RecurrenceMonthly : public Recurrence {
public:
	explicit RecurrenceMonthly(std::string s);
	virtual ~RecurrenceMonthly();
	virtual Date next();
	virtual std::string toString() const;
};
class RecurrenceYearly : public Recurrence {
public:
	explicit RecurrenceYearly(std::string s);
	virtual ~RecurrenceYearly();
	virtual Date next();
	virtual std::string toString() const;
};
class RecurrenceIntervalDays : public Recurrence {
public:
	explicit RecurrenceIntervalDays(std::string s);
	virtual ~RecurrenceIntervalDays();
	virtual Date next();
	virtual std::string toString() const;
};

class Duration {
	/* TODO
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

}

#endif // LIBGETODO_DATETIME_H
