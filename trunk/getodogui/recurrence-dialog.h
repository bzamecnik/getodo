// $Id$

#ifndef GETODOGUI_RECURRENCE_DIALOG_H
#define GETODOGUI_RECURRENCE_DIALOG_H

#include "getodo.h"
#include "getodogui.h"

// ----- class RecurrenceDialog --------------------

/** Recurrence editing dialog.
 * Usage: for editing an existing recurrence call setRecurrence(), for creating nothing.
 * Then call run() and get the new recurrence using getRecurrence().
 */
class RecurrenceDialog : public Gtk::Dialog
{
public:
	RecurrenceDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~RecurrenceDialog();

	/** Get recurrence.
	 * \return recurrence build upon contents of the form or RecurrenceOnce
	 * as a default if something went wrong
	 */
	getodo::Recurrence& getRecurrence();
	/** Set recurrence to edit.
	 * \param r recurrence to fill the form
	 */
	void setRecurrence(const getodo::Recurrence& r);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;

	/** Response signal handler.
	 * Process the form and build a recurrence.
	 *
	 * It is called when the dialog closes.
	 */
	void on_response(int response);

	/** Model columns for yearlyMonthCombobox. */
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
