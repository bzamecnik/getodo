// $Id$

#include "stdafx.h"
#include "tagmodel.h"

namespace getodo {

// ----- class TagListStore ---------------

TagListStore::TagListStore(TaskManager& _manager) : manager(_manager) {
	set_column_types(columns);
}

Glib::RefPtr<TagListStore> TagListStore::create(TaskManager& _manager) {
	TagListStore* store = new TagListStore(_manager);
	_manager.signal_tag_inserted.connect(sigc::mem_fun(*store, &TagListStore::on_tag_inserted));
	_manager.signal_tag_updated.connect(sigc::mem_fun(*store, &TagListStore::on_tag_updated));
	_manager.signal_tag_removed.connect(sigc::mem_fun(*store, &TagListStore::on_tag_removed));
	store->refresh();
	return Glib::RefPtr<TagListStore>(store);
}

void TagListStore::on_tag_inserted(Tag& tag) {
	insertTag(tag);
}

void TagListStore::on_tag_updated(Tag& tag) {
	Gtk::TreeModel::iterator iter = getIterById(tag.id);
	if (!iter) { return; }
	setRowFromTag(iter, tag);
	row_changed(get_path(iter), iter);
}

void TagListStore::on_tag_removed(Tag& tag) {
	eraseTag(tag);
}

void TagListStore::insertTag(Tag& tag) {
	Gtk::TreeModel::iterator iter = append();
	setRowFromTag(iter, tag);
}

void TagListStore::eraseTag(Tag& tag) {
	erase(getIterById(tag.id));
}

Gtk::TreeModel::iterator TagListStore::getIterById(id_t tagId) {
	Gtk::TreeNodeChildren childrenIter = children();
	Gtk::TreeModel::iterator iter;
	for (iter = childrenIter.begin(); iter != childrenIter.end(); ++iter) {
		if ((*iter)[columns.id] == tagId) {
			return iter;
		}
	}
	return childrenIter.end();
}

void TagListStore::setRowFromTag(Gtk::TreeModel::iterator& iter, Tag& tag) {
	// TODO: check if iter is ok
	Gtk::TreeModel::Row row = *iter;
	row[columns.id] = tag.id;
	row[columns.name] = tag.name;
}

// insert all tag from the task manager to the list store
void TagListStore::refresh() {
	clear();
	std::vector<Tag> tags = manager.getTags();
	foreach (Tag tag, tags) {
		insertTag(tag);
	}
}

} // namespace getodo
