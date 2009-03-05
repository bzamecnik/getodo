// $Id$

#ifndef FILTER_MODEL_H
#define FILTER_MODEL_H

#include "getodo.h"

namespace getodo {

/** Custom %FilterRule TreeStore.
 * 
 */
class FilterListStore : public Gtk::ListStore {
public:
	static Glib::RefPtr<FilterListStore> create(TaskManager& manager);

	class ModelColumns : public Gtk::TreeModel::ColumnRecord {
	public:
		Gtk::TreeModelColumn<int> id;
		Gtk::TreeModelColumn<Glib::ustring> name;

		ModelColumns() {
			add(id);
			add(name);
		}
	};

	ModelColumns columns;

	Gtk::TreeModel::iterator getIterById(id_t filterId);

protected:
	FilterListStore(TaskManager& manager);

	// called by underlying TaskManager
	void on_filter_inserted(FilterRule& filter);
	void on_filter_updated(FilterRule& filter);
	void on_filter_removed(FilterRule& filter);

	void setRowFromFilterRule(Gtk::TreeModel::iterator& iter, FilterRule& filter);
	void insertFilterRule(FilterRule& filter);
	void eraseFilterRule(FilterRule& filter);
	void refresh();
private:
	TaskManager& manager;
};

}

#endif FILTER_MODEL_H