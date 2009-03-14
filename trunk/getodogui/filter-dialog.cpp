// $Id$

#include "stdafx.h"
#include "filter-dialog.h"

namespace getodo {

// ----- class FilterDialog --------------------

FilterDialog::FilterDialog(BaseObjectType* cobject,
	const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Dialog(cobject), refXml(refGlade)
{
	Gtk::Button* buttonOk = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	Gtk::Button* buttonCancel = add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	refRuleTextBuffer = Gtk::TextBuffer::create();

	try {
		// child widgets
		refXml->get_widget("nameEntry", pNameEntry);
		refXml->get_widget("ruleTextview", pRuleTextview);
	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << "Error in instatiating widget from Glade file: " << e.what() << std::endl;
		exit(-1);
	}

	pRuleTextview->set_buffer(refRuleTextBuffer);
	clearPanel();
}

FilterDialog::~FilterDialog() {}

FilterRule FilterDialog::getFilterRule() {
	return filter;
}

void FilterDialog::setFilterRule(const FilterRule& filter) {
	this->filter = filter;
	pNameEntry->set_text(filter.name);
	refRuleTextBuffer->set_text(filter.rule);
}

void FilterDialog::on_response(int response) {
	if (response == Gtk::RESPONSE_OK) {
		// make a FilterRule from dialog contents
		filter.name = pNameEntry->get_text();
		filter.rule = refRuleTextBuffer->get_text();
	}
	hide();
	clearPanel();
}

void FilterDialog::clearPanel() {
	pNameEntry->set_text("");
	refRuleTextBuffer->set_text("");
}

} // namespace getodo
