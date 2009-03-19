// $Id$

#include "stdafx.h"
#include "filtermodel.h"

namespace getodo {

// ----- class FilterListStore ---------------

FilterListStore::FilterListStore(TaskManager& _manager) : manager(_manager) {
	set_column_types(columns);
}

Glib::RefPtr<FilterListStore> FilterListStore::create(TaskManager& _manager) {
	FilterListStore* store = new FilterListStore(_manager);
	_manager.signal_filter_inserted.connect(sigc::mem_fun(*store, &FilterListStore::on_filter_inserted));
	_manager.signal_filter_updated.connect(sigc::mem_fun(*store, &FilterListStore::on_filter_updated));
	_manager.signal_filter_removed.connect(sigc::mem_fun(*store, &FilterListStore::on_filter_removed));
	store->refresh();
	return Glib::RefPtr<FilterListStore>(store);
}

void FilterListStore::on_filter_inserted(FilterRule& filter) {
	insertFilterRule(filter);
}

void FilterListStore::on_filter_updated(FilterRule& filter) {
	Gtk::TreeModel::iterator iter = getIterById(filter.id);
	if (!iter) { return; }
	setRowFromFilterRule(iter, filter);
	row_changed(get_path(iter), iter);
}

void FilterListStore::on_filter_removed(FilterRule& filter) {
	eraseFilterRule(filter);
}

void FilterListStore::insertFilterRule(FilterRule& filter) {
	Gtk::TreeModel::iterator iter = append();
	setRowFromFilterRule(iter, filter);
}

void FilterListStore::eraseFilterRule(FilterRule& filter) {
	erase(getIterById(filter.id));
}

Gtk::TreeModel::iterator FilterListStore::getIterById(id_t filterId) {
	Gtk::TreeNodeChildren childrenIter = children();
	Gtk::TreeModel::iterator iter;
	for (iter = childrenIter.begin(); iter != childrenIter.end(); ++iter) {
		if ((*iter)[columns.id] == filterId) {
			return iter;
		}
	}
	return childrenIter.end();
}

void FilterListStore::setRowFromFilterRule(Gtk::TreeModel::iterator& iter, FilterRule& filter) {
	// TODO: check if iter is ok
	Gtk::TreeModel::Row row = *iter;
	row[columns.id] = filter.id;
	row[columns.name] = filter.name;
}

// insert all filter rules from the task manager to the list store
void FilterListStore::refresh() {
	clear();
	std::vector<FilterRule> filters = manager.getFilterRules();
	foreach (FilterRule filter, filters) {
		insertFilterRule(filter);
	}
}

} // namespace getodo
