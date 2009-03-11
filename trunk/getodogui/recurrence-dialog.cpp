// $Id$

#include "stdafx.h"
#include "recurrence-dialog.h"

// TODO:
// * make sensitive only controls for selected recurrence type, make others insensitive
//   * use setWidgetsInactive() and then handle radio buttons' change signals
// * set sensitivity for optional controls using checkbuttons


// ----- class RecurrenceDialog --------------------

RecurrenceDialog::RecurrenceDialog(BaseObjectType* cobject,
	const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Dialog(cobject), refXml(refGlade),
  recurrence(0)
{
	Gtk::Button* buttonOk = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	Gtk::Button* buttonCancel = add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	refMonthsTreeModel = Gtk::ListStore::create(monthsColumns);

	try {
		// child widgets
		refXml->get_widget("onceRadiobutton", onceRadiobutton);

		refXml->get_widget("dailyRadiobutton", dailyRadiobutton);
		refXml->get_widget("dailySpinbutton", dailySpinbutton);

		refXml->get_widget("weeklyRadiobutton", weeklyRadiobutton);
		refXml->get_widget("weeklySpinbutton", weeklySpinbutton);
		refXml->get_widget("weekdayMonCheckbutton", weekdayCheckbuttons["Mon"]);
		refXml->get_widget("weekdayTueCheckbutton", weekdayCheckbuttons["Tue"]);
		refXml->get_widget("weekdayWedCheckbutton", weekdayCheckbuttons["Wed"]);
		refXml->get_widget("weekdayThuCheckbutton", weekdayCheckbuttons["Thu"]);
		refXml->get_widget("weekdayFriCheckbutton", weekdayCheckbuttons["Fri"]);
		refXml->get_widget("weekdaySatCheckbutton", weekdayCheckbuttons["Sat"]);
		refXml->get_widget("weekdaySunCheckbutton", weekdayCheckbuttons["Sun"]);

		refXml->get_widget("monthlyRadiobutton", monthlyRadiobutton);
		refXml->get_widget("monthlySpinbutton", monthlySpinbutton);
		refXml->get_widget("monthlyDaySpinbutton", monthlyDaySpinbutton);

		refXml->get_widget("yearlyRadiobutton", yearlyRadiobutton);
		refXml->get_widget("yearlyCheckbutton", yearlyCheckbutton);
		refXml->get_widget("yearlyDaySpinbutton", yearlyDaySpinbutton);
		refXml->get_widget("yearlyMonthCombobox", yearlyMonthCombobox);

		refXml->get_widget("intervalRadiobutton", intervalRadiobutton);
		refXml->get_widget("intervalSinceEntry", intervalSinceEntry);
		refXml->get_widget("intervalUntilEntry", intervalUntilEntry);

		// signals
		//pTaskNewToolbutton->signal_clicked().connect(
		//	sigc::mem_fun(*this, &MainWindow::on_buttonTaskNew_clicked) );

	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << "Error in instatiating widget from Glade file: " << e.what() << std::endl;
		exit(-1);
	}

	yearlyMonthCombobox->set_model(refMonthsTreeModel);
	yearlyMonthCombobox->pack_start(monthsColumns.col_name);
	fillMonthsModel();
	clearPanel();
}

RecurrenceDialog::~RecurrenceDialog() {
	if (recurrence != 0) {
		delete recurrence;
		recurrence = 0;
	}
}


void RecurrenceDialog::on_response(int response) {
	using namespace getodo;
	if (response == Gtk::RESPONSE_OK) {
		// make a Recurrence from dialog contents
		if (recurrence != 0) {
			delete recurrence;
			recurrence = 0;
		}
		if (onceRadiobutton->get_active()) {
			recurrence = new RecurrenceOnce();
		} else if (dailyRadiobutton->get_active()) {
			recurrence = new RecurrenceDaily(dailySpinbutton->get_value_as_int());
		} else if (weeklyRadiobutton->get_active()) {
			RecurrenceWeekly::weekdaySet_t weekdays;
			weekdayCheckbuttons_t::iterator it;
			std::istringstream ss;
			for (it = weekdayCheckbuttons.begin();
				it != weekdayCheckbuttons.end(); ++it)
			{
				if (it->second->get_active()) {
					boost::gregorian::greg_weekday weekday(1);
					ss.str(it->first);
					ss >> weekday;
					if (!ss.fail()) {
						weekdays.insert(weekday);
					}
				}
			}
			recurrence = new RecurrenceWeekly(weeklySpinbutton->get_value_as_int(), weekdays);
		} else if (monthlyRadiobutton->get_active()) {
			if (monthlyDaySpinbutton->get_value_as_int() > 0) {
				recurrence = new RecurrenceMonthly(monthlySpinbutton->get_value_as_int(),
					monthlyDaySpinbutton->get_value_as_int());
			} else {
				recurrence = new RecurrenceMonthly(monthlySpinbutton->get_value_as_int());
			}
		} else if (yearlyRadiobutton->get_active()) {
			Gtk::TreeModel::iterator iter = yearlyMonthCombobox->get_active();
			if (yearlyCheckbutton->get_active() && iter && *iter) {
				boost::gregorian::partial_date partialDate(yearlyDaySpinbutton->get_value_as_int(),
					int((*iter)[monthsColumns.col_number]));
				recurrence = new RecurrenceYearly(partialDate);
			} else {
				recurrence = new RecurrenceYearly();
			}
		} else if (intervalRadiobutton->get_active()) {
			Date sinceDate(intervalSinceEntry->get_text());
			Date untilDate(intervalUntilEntry->get_text());
			// end date in one day after last day of interval
			// maybe use different type of intervale (open vs. closed)
			boost::gregorian::date_period period(sinceDate.date,
				untilDate.date + boost::gregorian::days(1));
			recurrence = new RecurrenceIntervalDays(period);
		}
	}
	hide();
	clearPanel();
}

getodo::Recurrence& RecurrenceDialog::getRecurrence() {
	if (recurrence == 0) {
		recurrence = new getodo::RecurrenceOnce();
	}
	return *recurrence;
}
void RecurrenceDialog::setRecurrence(const getodo::Recurrence& r) {
	using namespace getodo;
	if (recurrence != 0) {
		delete recurrence;
		recurrence = 0;
	}
	recurrence = r.clone();
	// set dialog contents according to recurrence
	std::string type = recurrence->getTypeLongName();
	if (type == "Once") {
		onceRadiobutton->set_active();
	} else if (type == "Daily") {
		dailyRadiobutton->set_active();
		RecurrenceDaily* daily = static_cast<RecurrenceDaily*>(recurrence);
		dailySpinbutton->set_value(daily->getPeriod());
	} else if (type == "Weekly") {
		weeklyRadiobutton->set_active();
		RecurrenceWeekly* weekly = static_cast<RecurrenceWeekly*>(recurrence);
		weeklySpinbutton->set_value(weekly->getPeriod());
		RecurrenceWeekly::weekdaySet_t weekdays = weekly->getWeekdaySelection();
		RecurrenceWeekly::weekdaySet_t::iterator it;
		for (it = weekdays.begin(); it != weekdays.end(); ++it) {
			weekdayCheckbuttons[it->as_short_string()]->set_active(true);
		}
	} else if (type == "Monthly") {
		monthlyRadiobutton->set_active();
		RecurrenceMonthly* monthly = static_cast<RecurrenceMonthly*>(recurrence);
		monthlySpinbutton->set_value(monthly->getPeriod());
		if (monthly->isDayOfMonthSet()) {
			monthlyDaySpinbutton->set_value(monthly->getDayOfMonth().as_number());
		}
	} else if (type == "Yearly") {
		yearlyRadiobutton->set_active();
		RecurrenceYearly* yearly = static_cast<RecurrenceYearly*>(recurrence);
		if (yearly->isDayAndMonthUsed()) {
			yearlyCheckbutton->set_active();
			boost::gregorian::partial_date dayAndMonth = yearly->getDayAndMonth();
			yearlyDaySpinbutton->set_value(dayAndMonth.day().as_number());
			// months in combo box numbered from 0
			yearlyMonthCombobox->set_active(dayAndMonth.month().as_number() - 1);
		}
	} else if (type == "Interval of days") {
		intervalRadiobutton->set_active();
		RecurrenceIntervalDays* interval = static_cast<RecurrenceIntervalDays*>(recurrence);
		intervalSinceEntry->set_text(Date(interval->getDateStart()).toString());
		intervalUntilEntry->set_text(Date(interval->getDateEnd()).toString());
	} else {
		onceRadiobutton->set_active();
	}
}

void RecurrenceDialog::fillMonthsModel() {
	refMonthsTreeModel->clear();
	for (int i = 1; i <= 12; i++) {
		Gtk::TreeModel::Row row = *(refMonthsTreeModel->append());
		row[monthsColumns.col_number] = i;
		boost::gregorian::greg_month month(i);
		row[monthsColumns.col_name] = month.as_long_string();
	}
}

void RecurrenceDialog::clearPanel() {
	onceRadiobutton->set_active();
	dailySpinbutton->set_value(1);
	weeklySpinbutton->set_value(1);
	{
		weekdayCheckbuttons_t::iterator it;
		for (it = weekdayCheckbuttons.begin();
			it != weekdayCheckbuttons.end(); ++it)
		{
			it->second->set_active(false);
		}
	}
	monthlySpinbutton->set_value(1);
	monthlyDaySpinbutton->set_value(0);
	yearlyCheckbutton->set_active(false);
	yearlyDaySpinbutton->set_value(1);
	yearlyMonthCombobox->set_active(0);
	intervalSinceEntry->set_text("");
	intervalUntilEntry->set_text("");
	// TODO: we need specific activating code for using this
	//setWidgetsInactive();
}

void RecurrenceDialog::setWidgetsInactive() {
	dailySpinbutton->set_sensitive(false);
	weeklySpinbutton->set_sensitive(false);
	{
		weekdayCheckbuttons_t::iterator it;
		for (it = weekdayCheckbuttons.begin();
			it != weekdayCheckbuttons.end(); ++it)
		{
			it->second->set_sensitive(false);
		}
	}
	monthlySpinbutton->set_sensitive(false);
	monthlyDaySpinbutton->set_sensitive(false);
	yearlyCheckbutton->set_sensitive(false);
	yearlyDaySpinbutton->set_sensitive(false);
	yearlyMonthCombobox->set_sensitive(false);
	intervalSinceEntry->set_sensitive(false);
	intervalUntilEntry->set_sensitive(false);
}