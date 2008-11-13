// $Id$

#ifndef GETODOGUI_RECURRENCE_DIALOG_H
#define GETODOGUI_RECURRENCE_DIALOG_H

#include "getodo.h"
#include "getodogui.h"

// ----- class RecurrenceDialog --------------------

// this dialog have to return a Recurrence

/** Recurrence editing dialog.
 */
class RecurrenceDialog : public Gtk::Dialog
{
public:
	RecurrenceDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	~RecurrenceDialog();

	getodo::Recurrence& getRecurrence();
	void setRecurrence(getodo::Recurrence& r);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;

	// ----- signal handlers -----
	void on_response(int response);

	// model columns for yearlyMonthCombobox
	class MonthsModelColumns : public Gtk::TreeModel::ColumnRecord
	{
		public:
		MonthsModelColumns() { add(col_number); add(col_name); }

		Gtk::TreeModelColumn<int> col_number;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
	};
	MonthsModelColumns monthsColumns;
	Glib::RefPtr<Gtk::ListStore> refMonthsTreeModel;

	// ----- child widgets ----
	Gtk::RadioButton* onceRadiobutton;

	Gtk::RadioButton* dailyRadiobutton;
	Gtk::SpinButton* dailySpinbutton;

	Gtk::RadioButton* weeklyRadiobutton;
	Gtk::SpinButton* weeklySpinbutton;
	typedef  std::map<std::string, Gtk::CheckButton*> weekdayCheckbuttons_t;
	weekdayCheckbuttons_t weekdayCheckbuttons;

	Gtk::RadioButton* monthlyRadiobutton;
	Gtk::SpinButton* monthlySpinbutton;
	Gtk::SpinButton* monthlyDaySpinbutton;

	Gtk::RadioButton* yearlyRadiobutton;
	Gtk::CheckButton* yearlyCheckbutton;
	Gtk::SpinButton* yearlyDaySpinbutton;
	Gtk::ComboBox* yearlyMonthCombobox;

	Gtk::RadioButton* intervalRadiobutton;
	Gtk::Entry* intervalSinceEntry;
	Gtk::Entry* intervalUntilEntry;
private:
	getodo::Recurrence* recurrence;

	void fillMonthsModel();
	void clearPanel();
	void setWidgetsInactive();
};

#endif // GETODOGUI_RECURRENCE_DIALOG_H
