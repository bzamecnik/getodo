// $Id$
//
// Class FilterDialog
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef GETODOGUI_FILTER_DIALOG_H
#define GETODOGUI_FILTER_DIALOG_H

#include "getodo.h"
#include "getodogui.h"

namespace getodo {

// ----- class FilterDialog --------------------

/** Filter rule editing dialog.
 * Usage: for editing an existing filter call setFilterRule(), for creating nothing.
 * Then call run() and get the new filter using getFilterRule().
 */
class FilterDialog : public Gtk::Dialog
{
public:
	FilterDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~FilterDialog();

	/** Get filter rule.
	 * \return filter rule build upon contents of the form
	 */
	FilterRule getFilterRule();
	/** Set filter rule to edit.
	 * \param filter filter rule to fill the form
	 */
	void setFilterRule(const FilterRule& filter);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;

	/** Response signal handler.
	 * Process the form and build a filter rule.
	 *
	 * It is called when the dialog closes.
	 */
	void on_response(int response);

	// ----- child widgets ----
	Gtk::Entry* pNameEntry;
	Gtk::TextView* pRuleTextview;
	Glib::RefPtr<Gtk::TextBuffer> refRuleTextBuffer;
private:
	FilterRule filter;

	void clearPanel();
};

} // namespace getodo

#endif // GETODOGUI_FILTER_DIALOG_H
