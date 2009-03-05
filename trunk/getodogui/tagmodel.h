// $Id$

#ifndef TAG_MODEL_H
#define TAG_MODEL_H

#include "getodo.h"
#include <sigc++/sigc++.h>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <string>

namespace getodo {

/** Custom %Tag ListStore.
 * 
 */
class TagListStore : public Gtk::ListStore {
public:
	static Glib::RefPtr<TagListStore> create(TaskManager& manager);

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
	TagListStore(TaskManager& manager);

	// called by underlying TaskManager
	void on_tag_inserted(Tag& tag);
	void on_tag_updated(Tag& tag);
	void on_tag_removed(Tag& tag);

	void setRowFromTag(Gtk::TreeModel::iterator& iter, Tag& tag);
	void insertTag(Tag& tag);
	void eraseTag(Tag& tag);
	void refresh();
private:
	TaskManager& manager;
};

} // namespace getodo

#endif // TAG_MODEL_H
