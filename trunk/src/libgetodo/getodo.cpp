// $Id$

#include "getodo.h"

namespace getodo {

// ----- class DateTime --------------------

DateTime::DateTime() {
	date = boost::posix_time::ptime(boost::date_time::not_a_date_time);
}
DateTime::DateTime(boost::posix_time::ptime const& date) {
	this->date = date;
}
DateTime::DateTime(DateTime const& date) {
	this->date = date.date;
}

// ----- class Date --------------------

Date Date::fromString(std::string str) {
	return boost::gregorian::from_string(str);
}

std::string Date::toString(Date const& date) {
	return boost::gregorian::to_iso_extended_string(date.date);
}

std::string Date::toString() {
	Date::toString(*this);
}

// ----- class Recurrence --------------------

// ----- class RecurrenceOnce --------------------

// ----- class RecurrenceDaily --------------------

// ----- class RecurrenceWeekly --------------------

// ----- class RecurrenceMonthly --------------------

// ----- class RecurrenceYearly --------------------

// ----- class RecurrenceIntervalDays --------------------

// ----- class Duration --------------------

}  // namespace getodo
