// $Id$
//
// Classes Recurrence, RecurrenceOnce, RecurrenceDaily, RecurrenceWeekly,
// RecurrenceMonthly, RecurrenceYearly, RecurrenceIntervalDays
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_RECURRENCE_H
#define LIBGETODO_RECURRENCE_H

#include "common.h"
#include "datetime.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <sstream>

namespace getodo {

class DateTime;
class Date;

/** %Recurrence.
 * Representation of repeating date events.
 * Abstract base for various recurrence types.
 *
 * Recurrence types:
 * - once - no recurrence (default)
 * - every Nth day
 * - every Nth week (optionally given selected weekdays)
 * - every Nth month (optionally given a day of month)
 * - every Nth year (optionally given a day and month)
 * - every day in an interval between two days
 *
 * Use next() funcion to obtain the date of next occurence of the event
 * after a date given. This can be used to generate repeated tasks.
 */
class Recurrence {
public:
	virtual ~Recurrence();

	/** Cloning support.
	 * Used as a virtual copy constructor.
	 * \return copy of this instance with respect to its type
	 */
	virtual Recurrence* clone() const = 0;

	/** Compute next occurence of the event.
	 * Compute the date of next occurence of the event after given date.
	 * 
	 * TODO: We can use boost::gregorian::date_iterator inside and return date
	 * or make a custom iterator.
	 *
	 * \param start date after which the event happens
	 * \return date of next occurence of the event
	 */
	virtual Date next(Date start) = 0;
	
	/** Serialization with atype identifier.
	 * Convert a recurrence to string.
	 *
	 * Note: this function adds a correct recurrence type identifier as a prefix.
	 *
	 * \param r recurrence
	 * \return serialized form of recurrence with type identifier
	 */
	static std::string toString(const Recurrence& r);

	/** Serialization without any type identifier.
	 * Convert a recurrence to string.
	 *
	 * \return serialized form of recurrence
	 */
	std::string toString() const;

	/** Deserialization.
	 * Given a string create proper Recurrence* object using
	 * recurrence type identifier. It is a factory method.
	 * RecurrenceOnce is a default fallback in case of bad syntax
	 * of the string.
	 *
	 * \param str serilized recurrence with type identifier
	 * \return deserialized recurrence or RecurrenceOnce on error
	 */
	static Recurrence* fromString(std::string str);

	/** Print on an output stream.
	 * Use virtual printOn() for correct serializatoin.
	 * \param o output stream
	 * \param r recurrence to print
	 * \return modified output stream
	 */
	friend std::ostream& operator<< (std::ostream& o, const Recurrence& r);

	bool operator==(const Recurrence& rhs) const;
	bool operator!=(const Recurrence& rhs) const;

	/** Human readable recurrence type name.
	 * Used in the user interface.
	 * \return long name of the recurrence type
	 */
	virtual std::string getTypeLongName() const = 0;
protected:
	/** Type identifier for serialization.
	 * \return identifier of recurrence type
	 */
	virtual std::string getTypeId() const = 0;

	/** Print on output stream.
	 * As operator<<() cannot be virtual the core code is separated into
	 * printOn(). It is also used is toString() functions.
	 */
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
	/** Serialization. Eg. "" */
	virtual void printOn(std::ostream& o) const;
};

/** Daily recurrence.
 * Occurs every n-th day. The n is refered as \p period.
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

	/** Period in days.
	 * \return period in days
	 */
	int getPeriod() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "2" */
	virtual void printOn(std::ostream& o) const;
};

/** Weekly recurrence.
 * Occurs every n-th week. The n is refered as \p period.
 * Two modes can be used:
 * - every n-th week after a specified date
 * - every n-th week using a selection of weekdays
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

	/** Period in weeks.
	 * \return period in weeks
	 */
	int getPeriod() const;

	/** Weekday selection.
	 * On which days of week the event can happen. If empty the recurrence
	 * is computed on that weekday as in the date given.
	 * \return set of weekdays
	 */
	weekdaySet_t getWeekdaySelection() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "1 Mon Tue" */
	virtual void printOn(std::ostream& o) const;
};

/** Monthly recurrence.
 * Occurs every n-th month. The n is refered as \p period.
 * Two modes can be used:
 * - every n-th month after a specified date
 * - every n-th month on a given day of month.
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

	/** Period in months.
	 * \return period in months
	 */
	int getPeriod() const;

	/** Day of month.
	 * \return day of month on which the event occurs.
	 */
	boost::gregorian::greg_day getDayOfMonth() const;

	/** Check if the day of month is used.
	 * That is to check the mode of operation.
	 * \return true if day of month is set and used
	 */
	bool isDayOfMonthSet() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "3 14" */
	virtual void printOn(std::ostream& o) const;
};

/** Yearly recurrence.
 * Occurs every year. Two modes can be used:
 * - every year after a specified date
 * - every year on given day and month
 */
class RecurrenceYearly : public Recurrence {
private:
	boost::gregorian::partial_date dayAndMonth;
	/** A switch for optionally specified day and month:
	 *  * true - next date will be on useDayAndMonth next year
	 *  * false - next date will be a year after date given in next()
	 */
	bool useDayAndMonth;
public:
	RecurrenceYearly();
	RecurrenceYearly(boost::gregorian::partial_date dayAndMonth);
	explicit RecurrenceYearly(std::string s);
	RecurrenceYearly(const RecurrenceYearly& r);
	virtual ~RecurrenceYearly();
	virtual RecurrenceYearly* clone() const;
	virtual Date next(Date start);

	/** Day and month of year.
	 * \return day and month of year
	 */
	boost::gregorian::partial_date getDayAndMonth() const;

	/** Check if day and month of year is used.
	 * \return true if day and month of year is set and used
	 */
	bool isDayAndMonthUsed() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "25 Dec" */
	virtual void printOn(std::ostream& o) const;
};

/** %Recurrence on interval of days.
 * Occurs every day between two dates.
 * If input is bad next() will return not_a_date_time,
 * just like RecurrenceOnce::next().
 * Date period will be invalid, eg. of zero length.
 */
class RecurrenceIntervalDays : public Recurrence {
private:
	boost::gregorian::date_period interval;
public:
	RecurrenceIntervalDays(boost::gregorian::date_period datePeriod);
	explicit RecurrenceIntervalDays(std::string s);
	RecurrenceIntervalDays(const RecurrenceIntervalDays& r);
	virtual ~RecurrenceIntervalDays();
	virtual RecurrenceIntervalDays* clone() const;
	virtual Date next(Date start);

	/** Date of period start.
	 * \return date of period start
	 */
	boost::gregorian::date getDateStart() const;
	/** Date of period end.
	 * \return date of period end
	 */
	boost::gregorian::date getDateEnd() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const;
};
	
}

#endif // LIBGETODO_RECURRENCE_H
