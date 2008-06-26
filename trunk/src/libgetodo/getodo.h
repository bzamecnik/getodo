// $Id: getodo.h 11 2008-06-25 13:20:57Z bohumir.zamecnik $
//
// Base libgetodo header file.
//
// Description: classes DateTime, Date, Recurrence*, Duration
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef LIBGETODO_GETODO_H
#define LIBGETODO_GETODO_H

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

// THINK: make AbstractDate a common ancestor to Date and DateTime?

class DateTime {
public:
	boost::posix_time::ptime date;
	
	DateTime();
	DateTime(boost::posix_time::ptime const& date);
	DateTime(DateTime const& date);
	
	// For database storage.
	// Format: YYYY-MM-DD HH:MM:SS
	static DateTime fromString(std::string str);
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

} // namespace getodo

#endif // LIBGETODO_GETODO_H
