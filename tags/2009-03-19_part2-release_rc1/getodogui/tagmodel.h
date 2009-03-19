// $Id$
//
// Class TagListStore
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef GETODOGUI_TAG_MODEL_H
#define GETODOGUI_TAG_MODEL_H

#include "getodo.h"
#include <sigc++/sigc++.h>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <string>

namespace getodo {

/** Custom %Tag ListStore.
 * In addition to ListStore TagListStore contains a reference to the 
 * TaskManager, it responds to signals from it and change itself according
 * to what is in the TaskManager. Tags can be localized in the model
 * using the getIterById() function.
 * 
 * TaskTreeStore is constructed using create() factory method.
 */
class TagListStore : public Gtk::ListStore {
public:
	/** Create a new TagListStore.
	 * A factory method. Its purpose is to wrap it with Glib::RefPtr.
	 * \return RefPtr to the TagListStore instance
	 */
	static Glib::RefPtr<TagListStore> create(TaskManager& manager);

	/** %TagListStore model columns.
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

	/** Find tag in the model.
	 * Search for the tag by its id in the tree and return an iterator
	 * to row where it is.
	 * \param tagId id of the tag to find
	 * \return iterator to the tag or children().end() if nothing found
	 */
	Gtk::TreeModel::iterator getIterById(id_t tagId);

protected:
	TagListStore(TaskManager& manager);

	/** Tag insertion signal handler.
	 * Called by signal_tag_inserted of the underlying TaskManager.
	 * \param tag tag being inserted
	 */
	void on_tag_inserted(Tag& tag);

	/** Tag update signal handler.
	 * Called by signal_tag_updated of the underlying TaskManager.
	 * \param tag tag being updated
	 */
	void on_tag_updated(Tag& tag);

	/** Tag deletion signal handler.
	 * Called by signal_tag_deleted of the underlying TaskManager.
	 * \param tag tag being deleted
	 */
	void on_tag_removed(Tag& tag);

	/** Set the model row according to tag contents.
	 * \param iter iterator pointing to the row
	 * \param tag tag using which to fill the row
	 */
	void setRowFromTag(Gtk::TreeModel::iterator& iter, Tag& tag);

	/** Insert tag into the model.
	 * Insert the tag into the model.
	 *
	 * Called by the signal handler.
	 *
	 * \param tag tag to be inserted
	 */
	void insertTag(Tag& tag);

	/** Delete tag from the model.
	 * Find the tag in the model tree and delete it.
	 *
	 * Called by the signal handler.
	 *
	 * \param tag tag to be deleted
	 */
	void eraseTag(Tag& tag);

	/** Refresh the model.
	 * Clean the model and insert all the tags from the TaskManager
	 * in a batch.
	 */
	void refresh();
private:
	TaskManager& manager;
};

} // namespace getodo

#endif // GETODOGUI_TAG_MODEL_H
