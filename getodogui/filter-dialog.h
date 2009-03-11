// $Id: $

#ifndef GETODOGUI_FILTER_DIALOG_H
#define GETODOGUI_FILTER_DIALOG_H

#include "getodo.h"
#include "getodogui.h"

// ----- class FilterDialog --------------------

/** Filter rule editing dialog.
 */
class FilterDialog : public Gtk::Dialog
{
public:
	FilterDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~FilterDialog();

	getodo::FilterRule getFilterRule();
	void setFilterRule(const getodo::FilterRule& filter);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;

	// ----- signal handlers -----
	void on_response(int response);

	// ----- child widgets ----
	Gtk::Entry* pNameEntry;
	Gtk::TextView* pRuleTextview;
	Glib::RefPtr<Gtk::TextBuffer> refRuleTextBuffer;
private:
	getodo::FilterRule filter;

	void clearPanel();
};

#endif // GETODOGUI_FILTER_DIALOG_H
