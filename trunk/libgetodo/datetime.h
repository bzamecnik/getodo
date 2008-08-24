// $Id$
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

#include "common.h"

#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>

namespace getodo {

// THINK:
//	* Make AbstractDate a common ancestor to Date and DateTime?
//  * How to cope with not_a_date_time invalid dates from bad input strings?

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
	// Recurrence types:
	//   * once - no recurrence - default
	//   * every Nth day
	//   * every Nth week - optionally given selected weekdays
	//   * every Nth month - optionally given a day of month
	//   * every Nth year - optionally given a day and month
	//   * every day in an interval between two days
public:
	virtual ~Recurrence();

	// Maybe use boost::gregorian::date_iterator inside and return date
	// or make a custom iterator
	virtual Date next(Date start)=0;
	virtual std::string toString()const =0;
	
	// Prepend correct recurrence type identifier
	static std::string toString(const Recurrence& r);
	// Given a string create proper Recurrence* object using
	// recurrence type identifier
	static Recurrence* fromString(std::string str);
protected:
	virtual std::string getTypeId()const =0;
};

class RecurrenceOnce : public Recurrence {
public:
	RecurrenceOnce();
	virtual ~RecurrenceOnce();
	virtual Date next(Date start);
	virtual std::string toString() const; // eg. ""
protected:
	virtual std::string getTypeId() const;
};

class RecurrenceDaily : public Recurrence {
private:
	int period;
public:
	explicit RecurrenceDaily(std::string s);
	virtual ~RecurrenceDaily();
	virtual Date next(Date start);
	virtual std::string toString() const; // eg. "2"
protected:
	virtual std::string getTypeId() const;
};

class RecurrenceWeekly : public Recurrence {
private:
	int period;
	typedef std::set<boost::gregorian::greg_weekday> weekdaySet_t;
	weekdaySet_t weekdaySelection;
	// Switch for optionally specified weekday selection:
	// true - next date will be computed using weekday selection
	// false - next date will be a wekk after date given in next()
	bool useWeekdaySelection;
public:
	explicit RecurrenceWeekly(std::string s);
	virtual ~RecurrenceWeekly();
	virtual Date next(Date start);
	virtual std::string toString() const; // eg. "1 Mon Tue"
protected:
	virtual std::string getTypeId() const;
};

class RecurrenceMonthly : public Recurrence {
private:
	int period;
	boost::gregorian::greg_day dayOfMonth;
	// Switch for optionally specified day of month:
	// true - next date will be on dayOfMonth next month
	// false - next date will be a month after date given in next()
	bool useDayOfMonth;
public:
	explicit RecurrenceMonthly(std::string s);
	virtual ~RecurrenceMonthly();
	virtual Date next(Date start);
	virtual std::string toString() const; // eg. "3 14"
protected:
	virtual std::string getTypeId() const;
};

class RecurrenceYearly : public Recurrence {
private:
	boost::gregorian::partial_date dayAndMonth;
	// Switch for optionally specified day and month:
	// true - next date will be on useDayAndMonth next year
	// false - next date will be a year after date given in next()
	bool useDayAndMonth;
public:
	explicit RecurrenceYearly(std::string s);
	virtual ~RecurrenceYearly();
	virtual Date next(Date start);
	virtual std::string toString() const; // eg. "25 Dec"
protected:
	virtual std::string getTypeId() const;
};

class RecurrenceIntervalDays : public Recurrence {
private:
	boost::gregorian::date_period interval;
public:
	// If input is bad, next() will return not_a_date_time,
	// just like RecurrenceOnce::next().
	// Period will be invalid, eg. of zero length.
	explicit RecurrenceIntervalDays(std::string s);
	virtual ~RecurrenceIntervalDays();
	virtual Date next(Date start);
	virtual std::string toString() const;
protected:
	virtual std::string getTypeId() const;
};

class Duration {
	/* TODO
	Duration should express:
		* units of given magnitude (minutes, hours, days, weeks, months, years)
		* - lesser magnitude => greater precision
		* or no information
	Use:
		boost::gregorian::years, boost::gregorian::months, boost::gregorian::weeks
		boost::gregorian::date_duration (boost::gregorian::days)
		boost::posix_time::hours, boost::posix_time::minutes, boost::posix_time::seconds
	*/
};

}

#endif // LIBGETODO_DATETIME_H
