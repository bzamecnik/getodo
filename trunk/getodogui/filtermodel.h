// $Id$
//
// Class FilterListStore
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef GETODOGUI_FILTER_MODEL_H
#define GETODOGUI_FILTER_MODEL_H

#include "getodo.h"

namespace getodo {

/** Custom %FilterRule ListStore.
 * In addition to ListStore FilterListStore contains a reference to the 
 * TaskManager, it responds to signals from it and change itself according
 * to what is in the TaskManager. Filter rules can be localized in the model
 * using the getIterById() function.
 * 
 * FilterTreeStore is constructed using create() factory method.
 */
class FilterListStore : public Gtk::ListStore {
public:
	/** Create a new FilterListStore.
	 * A factory method. Its purpose is to wrap it with Glib::RefPtr.
	 * \return RefPtr to the FilterListStore instance
	 */
	static Glib::RefPtr<FilterListStore> create(TaskManager& manager);

	/** %FilterListStore model columns.
	 */
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

	/** Find filter rule in the model.
	 * Search for the filter rule by its id in the tree and return an iterator
	 * to row where it is.
	 * \param filterId id of the filter rule to find
	 * \return iterator to the filter rule or children().end() if nothing found
	 */
	Gtk::TreeModel::iterator getIterById(id_t filterId);

protected:
	FilterListStore(TaskManager& manager);

	/** Filter rule insertion signal handler.
	 * Called by signal_filter_inserted of the underlying TaskManager.
	 * \param filter filter rule being inserted
	 */
	void on_filter_inserted(FilterRule& filter);

	/** Filter rule update signal handler.
	 * Called by signal_filter_updated of the underlying TaskManager.
	 * \param filter filter rule being updated
	 */
	void on_filter_updated(FilterRule& filter);

	/** Filter rule deletion signal handler.
	 * Called by signal_filter_deleted of the underlying TaskManager.
	 * \param filter filter rule being deleted
	 */
	void on_filter_removed(FilterRule& filter);

	/** Set the model row according to filter rule contents.
	 * \param iter iterator pointing to the row
	 * \param filter filter rule using which to fill the row
	 */
	void setRowFromFilterRule(Gtk::TreeModel::iterator& iter, FilterRule& filter);

	/** Insert filter rule into the model.
	 * Insert the filter rule into the model.
	 *
	 * Called by the signal handler.
	 *
	 * \param filter filter rule to be inserted
	 */
	void insertFilterRule(FilterRule& filter);

	/** Delete filter rule from the model.
	 * Find the filter rule in the model tree and delete it.
	 *
	 * Called by the signal handler.
	 *
	 * \param filter filter rule to be deleted
	 */
	void eraseFilterRule(FilterRule& filter);

	/** Refresh the model.
	 * Clean the model and insert all the filter rules from the TaskManager
	 * in a batch.
	 */
	void refresh();
private:
	TaskManager& manager;
};

} // namespace getodo

#endif // GETODOGUI_FILTER_MODEL_H
