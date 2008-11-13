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

/** %Date and time.
 * Representaion of date and time.
 */
class DateTime {
public:
	boost::posix_time::ptime date;
	
	DateTime();
	DateTime(const boost::posix_time::ptime& d);
	DateTime(const DateTime& d); // copy
	explicit DateTime(std::string d);
	// explicit DateTime(Date d);
	
	static DateTime now();

	std::string toString() const;
	friend std::ostream& operator<< (std::ostream& o, const DateTime& date);
private:
	// format for string representation
	// eg. YYYY-MM-DD HH:MM:SS
	static std::string format;
};

/** %Date.
 * Representaion of date.
 */
class Date {
public:
	boost::gregorian::date date;
	
	Date();
	Date(const boost::gregorian::date& d);
	Date(const Date& d);
	explicit Date(std::string d);

	static Date now();
	
	// Format: YYYY-MM-DD
	std::string toString() const;
	friend std::ostream& operator<< (std::ostream& o, const Date& date);
private:
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

/** %Recurrence.
 * Representation of repeating date events.
 * Abstract base for various recurrence types.
 */
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
	virtual Recurrence* clone() const = 0;

	// Maybe use boost::gregorian::date_iterator inside and return date
	// or make a custom iterator
	virtual Date next(Date start)=0;
	
	// Prepend correct recurrence type identifier
	static std::string toString(const Recurrence& r);
	// Without type identifier
	std::string toString() const;
	// Given a string create proper Recurrence* object using
	// recurrence type identifier.
	// This is a kind of Factory Method (hope).
	static Recurrence* fromString(std::string str);

	friend std::ostream& operator<< (std::ostream& o, const Recurrence& r);
	virtual std::string getTypeLongName() const = 0;
protected:
	virtual std::string getTypeId() const = 0;
	virtual void printOn(std::ostream& o) const = 0;
};

/** One time recurrence.
 * Recurrence with no repeating.
 */
class RecurrenceOnce : public Recurrence {
public:
	RecurrenceOnce();
	RecurrenceOnce(const RecurrenceOnce& r);
	virtual ~RecurrenceOnce();
	virtual RecurrenceOnce* clone() const;
	virtual Date next(Date start);
	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const; // eg. ""
};

/** Daily recurrence.
 * Occurs every n-th day.
 */
class RecurrenceDaily : public Recurrence {
private:
	int period;
public:
	RecurrenceDaily(int period);
	explicit RecurrenceDaily(std::string s);
	RecurrenceDaily(const RecurrenceDaily& r);
	virtual ~RecurrenceDaily();
	virtual RecurrenceDaily* clone() const;
	virtual Date next(Date start);

	int getPeriod() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const; // eg. "2"
};

/** Weelky recurrence.
 * Occurs every week. Two modes can be used: every week after specified date
 * or using a selection of weekdays.
 */
class RecurrenceWeekly : public Recurrence {
public:
	typedef std::set<boost::gregorian::greg_weekday> weekdaySet_t;
private:
	int period;
	weekdaySet_t weekdaySelection;
	// Switch for optionally specified weekday selection:
	// true - next date will be computed using weekday selection
	// false - next date will be a week after date given in next()
	bool useWeekdaySelection;
public:
	RecurrenceWeekly(int period);
	RecurrenceWeekly(int period, weekdaySet_t weekdays);
	explicit RecurrenceWeekly(std::string s);
	RecurrenceWeekly(const RecurrenceWeekly& r);
	virtual ~RecurrenceWeekly();
	virtual RecurrenceWeekly* clone() const;
	virtual Date next(Date start);

	int getPeriod() const;
	weekdaySet_t getWeekdaySelection() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const; // eg. "1 Mon Tue"
};

/** Monthly recurrence.
 * Occurs every month. Two modes can be used: every month after specified date
 * or on given day of month.
 */
class RecurrenceMonthly : public Recurrence {
private:
	int period;
	boost::gregorian::greg_day dayOfMonth;
	// Switch for optionally specified day of month:
	// true - next date will be on dayOfMonth next month
	// false - next date will be a month after date given in next()
	bool useDayOfMonth;
public:
	RecurrenceMonthly(int period);
	RecurrenceMonthly(int period, boost::gregorian::greg_day dayOfMonth);
	explicit RecurrenceMonthly(std::string s);
	RecurrenceMonthly(const RecurrenceMonthly& r);
	virtual ~RecurrenceMonthly();
	virtual RecurrenceMonthly* clone() const;
	virtual Date next(Date start);
	
	int getPeriod() const;
	boost::gregorian::greg_day getDayOfMonth() const;
	bool isDayOfMonthSet() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const; // eg. "3 14"
};

/** Yearly recurrence.
 * Occurs every year. Two modes can be used: every year after specified date
 * or on given day and month of year.
 */
class RecurrenceYearly : public Recurrence {
private:
	boost::gregorian::partial_date dayAndMonth;
	// Switch for optionally specified day and month:
	// true - next date will be on useDayAndMonth next year
	// false - next date will be a year after date given in next()
	bool useDayAndMonth;
public:
	RecurrenceYearly();
	RecurrenceYearly(boost::gregorian::partial_date dayAndMonth);
	explicit RecurrenceYearly(std::string s);
	RecurrenceYearly(const RecurrenceYearly& r);
	virtual ~RecurrenceYearly();
	virtual RecurrenceYearly* clone() const;
	virtual Date next(Date start);
	
	boost::gregorian::partial_date getDayAndMonth() const;
	bool isDayAndMonthUsed() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const; // eg. "25 Dec"
};

/** %Recurrence on interval of days.
 * Occurs every day between two dates.
 */
class RecurrenceIntervalDays : public Recurrence {
private:
	boost::gregorian::date_period interval;
public:
	// If input is bad, next() will return not_a_date_time,
	// just like RecurrenceOnce::next().
	// Period will be invalid, eg. of zero length.
	RecurrenceIntervalDays(boost::gregorian::date_period datePeriod);
	explicit RecurrenceIntervalDays(std::string s);
	RecurrenceIntervalDays(const RecurrenceIntervalDays& r);
	virtual ~RecurrenceIntervalDays();
	virtual RecurrenceIntervalDays* clone() const;
	virtual Date next(Date start);

	boost::gregorian::date getDateStart() const;
	boost::gregorian::date getDateEnd() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const;
};

/** %Duration.
 * Representation of a time interval with given magnitude.
 * Not implemented yet.
 */
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
