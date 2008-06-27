// $Id$

#include "getodo.h"

namespace getodo {

// ----- class DateTime --------------------

//DateTime::DateTime() : date(boost::posix_time::ptime(boost::date_time::not_a_date_time)) {}
DateTime::DateTime() {}
DateTime::DateTime(const boost::posix_time::ptime& d) : date(d) {}
DateTime::DateTime(const DateTime& d) : date(d.date) {}

DateTime DateTime::fromString(std::string str) {
	return boost::posix_time::time_from_string(str);
}

std::string DateTime::toString(const DateTime& date) {
	return boost::posix_time::to_iso_extended_string(date.date);
}

std::string DateTime::toString() const {
	DateTime::toString(*this);
}

// ----- class Date --------------------

//Date::Date() : date(boost::gregorian::date(boost::date_time::not_a_date_time)) {}
Date::Date() {}
Date::Date(const boost::gregorian::date& d) : date(d) {}
Date::Date(const Date& d) : date(d.date) {}

Date Date::fromString(std::string str) {
	return boost::gregorian::from_string(str);
}

std::string Date::toString(const Date& date) {
	return boost::gregorian::to_iso_extended_string(date.date);
}

std::string Date::toString() const { Date::toString(*this); }

// ----- class Recurrence --------------------

// ----- class RecurrenceOnce --------------------

// ----- class RecurrenceDaily --------------------

// ----- class RecurrenceWeekly --------------------

// ----- class RecurrenceMonthly --------------------

// ----- class RecurrenceYearly --------------------

// ----- class RecurrenceIntervalDays --------------------

// ----- class Duration --------------------

}  // namespace getodo
