// $Id$
//
// Description: classes DateTime, Date, Duration
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
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
//	- Make AbstractDate a common ancestor to Date and DateTime?
//  - How to cope with not_a_date_time invalid dates from bad input strings?

// TODO:
// - Support operator==() on DateTime, Date
//	 It would be useful in TaskPersistence.

/** %Date and time.
 * Representaion of date and time. We have own DateTime class instead of
 * using boost::posix_time::ptime because of serialization to own string 
 * format. In future DateTime, Date and possibly FuzzyDate could have
 * a common abstract base.
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

	/** Serialization */
	std::string toString() const;
	/** Deserialization */
	friend std::ostream& operator<< (std::ostream& o, const DateTime& date);
private:
	// format for string representation
	// eg. YYYY-MM-DD HH:MM:SS
	static std::string format;
};

/** %Date.
 * Representaion of date. We have own Date class instead of
 * using boost::gregorian::date because of serialization to own string 
 * format.
 */
class Date {
public:
	boost::gregorian::date date;
	
	Date();
	Date(const boost::gregorian::date& d);
	Date(const Date& d);
	explicit Date(std::string d);

	static Date now();
	
	/** Serialization */
	std::string toString() const;
	/** Deserialization */
	friend std::ostream& operator<< (std::ostream& o, const Date& date);
private:
	// Format: YYYY-MM-DD
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
