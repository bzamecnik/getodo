// $Id$

#include "stdafx.h"
#include "tagmodel.h"

namespace getodo {

// ----- class TagNode --------------------

TagNode::~TagNode() {
	for (TagNodeVector::iterator it = children.begin();
		it != children.end(); ++it)
	{
		delete *it;
	}
}

//// ----- class TagManager --------------------
//
//TagManager::TagManager() {}
//TagManager::~TagManager() {
//	for(tags_t::iterator it = tags.begin(); it != tags.end(); ++it) {
//		if(it->second) {
//			delete it->second;
//			it->second = 0;
//		}
//	}
//}
//void TagManager::insertTag(Tag* tag) {
//	using namespace std;
//	if (tag) {
//		pair<tags_t::iterator, bool> result = tags.insert(make_pair(tag->id, tag));
//		if (result.second) {
//			signal_tag_inserted(*tag);
//		}
//	}
//}
//bool TagManager::hasTag(id_t tagId) {
//	return tags.find(tagId) != tags.end();
//}
//Tag* TagManager::getTag(id_t tagId) {
//	tags_t::iterator foundTag = tags.find(tagId);
//	if (foundTag != tags.end()) {
//		return foundTag->second;
//	} else {
//		return 0;
//	}
//}
//void TagManager::updateTag(Tag* tag) {
//	if (!tag) { return; }
//	tags_t::iterator updatedTag = tags.find(tag->id);
//	if (updatedTag != tags.end()) {
//		tags[tag->id]->name = tag->name;
//		signal_tag_updated(*tag);
//	} else {
//		insertTag(tag);
//	}
//	
//}
//void TagManager::removeTag(id_t tagId) {
//	tags_t::iterator tagToRemove = tags.find(tagId);
//	if(tagToRemove != tags.end()) {
//		signal_tag_removed(*(tagToRemove->second));
//		delete tagToRemove->second;
//		tags.erase(tagToRemove);
//	}
//}
//
//std::list<Tag*> TagManager::getTagsList() {
//	std::list<Tag*> tagList;
//	for (tags_t::iterator it = tags.begin(); it != tags.end(); ++it) {
//		tagList.push_back(it->second);
//	}
//	return tagList;
//}

// ----- class TagModel --------------------

TagModel::TagModel(TaskManager& _manager)
: manager(_manager)
{
	manager.signal_tag_inserted.connect(sigc::mem_fun(*this, &TagModel::on_tag_inserted));
	manager.signal_tag_updated.connect(sigc::mem_fun(*this, &TagModel::on_tag_updated));
	manager.signal_tag_removed.connect(sigc::mem_fun(*this, &TagModel::on_tag_removed));

	refresh();
}

TagModel::~TagModel() {
	for (TagNodeMap::iterator it = tagNodeMap.begin(); it != tagNodeMap.end(); ++it) {
		delete it->second;
		it->second = 0;
	}
}

TagNodeVector& TagModel::get_tags(void) const {
	return const_cast<TagNodeVector&>(tagNodes);
}

TagNode& TagModel::get_next_node(TagNode& node) const {
	TagNodeVector::const_iterator it = std::find(tagNodes.begin(), tagNodes.end(), &node);
	if (it == tagNodes.end() || (it+1) == tagNodes.end())
        throw InvalidNode();
    return **(++it);
}

TagNode& TagModel::get_node(Path& path) const {
	// this is for lists only
	// TODO: remake for trees
	if (path.size() < 1) {
		throw InvalidPath();
	}
	
	TagNodeVector::const_iterator it = tagNodes.begin();
	std::advance(it, path[0]);
	if (it == tagNodes.end()) {
		throw InvalidPath();
	}
	TagNode* node = *it;
	if (!node) {
		throw InvalidPath();
	}
	return *node;
}

TagModel::Path TagModel::get_path(TagNode& node) const {
	// TODO: remake for trees
	Path path(1);
	path[0] = (int)std::distance(tagNodes.begin(),
		std::find(tagNodes.begin(), tagNodes.end(), &node));
	//if (path[0] > 0) {
	//	--path[0];
	//}
	return path;
}

void TagModel::refresh(void) {
	clear();
	std::list<Tag*> tagList = manager.getTagsList();
	for (std::list<Tag*>::iterator it = tagList.begin(); it != tagList.end(); ++it) {
		insert(**it);
	}
}

void TagModel::clear(void) {
	for (TagNodeMap::iterator it = tagNodeMap.begin();
		it != tagNodeMap.end(); ++it)
	{
		TagNode* node = it->second;
		signal_node_removed(*node, get_path(*node));
		delete node;
	}
	tagNodeMap.clear();
	tagNodes.clear();
}

void TagModel::remove(Tag& tag) {
	TagNodeMap::iterator mapIt = tagNodeMap.find(tag.id);
	if (mapIt == tagNodeMap.end()) { return; }
	
	TagNode* node = mapIt->second;
	TagNodeVector::iterator tagNodeIt = std::find(tagNodes.begin(), tagNodes.end(), node);
	
	signal_node_removed(*node, get_path(*node));
	tagNodes.erase(tagNodeIt);
	tagNodeMap.erase(mapIt);
}

void TagModel::insert(Tag& tag) {
	TagNode* newNode = new TagNode(tag);
	tagNodes.push_back(newNode);
	std::pair<TagNodeMap::iterator,bool> result = tagNodeMap.insert(std::make_pair(tag.id, newNode));
	if (result.second) {
		Path newPath(1);
		newPath[0] = (int)std::distance(tagNodes.begin(),
			std::find(tagNodes.begin(), tagNodes.end(), newNode));
		signal_node_inserted(*newNode, newPath);
	}
}

void TagModel::on_tag_inserted(Tag& tag) {
	// called by underlying TagManager
	insert(tag);
}

void TagModel::on_tag_updated(Tag& tag) {
	// called by underlying TagManager
	TagNodeMap::iterator mapIt = tagNodeMap.find(tag.id);
	if (mapIt != tagNodeMap.end()) {
		TagNode* node = mapIt->second;
		Path path = get_path(*node);
		// Don't delete the node!
		// * Just try to replace its content while having the
		// children or parent pointer untouched.

		// BUGFIX: Commenting the following code seem
		// to fix a bug, when the program hung with an old
		// iterator pointing to deleted node.
		// Is this ok?

		//if (node) {
		//	delete node;
		//	node = 0;
		//}
		//mapIt->second = node = new TagNode(tag);
		signal_node_updated(*node, path);
	} else {
		insert(tag);
	}
}

void TagModel::on_tag_removed(Tag& tag) {
	// called by underlying TagManager
	remove(tag);
}

// ----- class TagTreeModel --------------------

TagTreeModel_Class TagTreeModel::tagtreemodel_class_;

TagTreeModel::TagTreeModel(TaskManager& manager)
: Glib::ObjectBase("GeToDo_TagTreeModel"), 
  Glib::Object(Glib::ConstructParams(tagtreemodel_class_.init(), (char*) 0)),
  TreeModel(),
  model(manager), stamp(1)
{
	model.signal_node_inserted.connect(sigc::mem_fun(*this, &TagTreeModel::on_treemodel_inserted));
	model.signal_node_updated.connect(sigc::mem_fun(*this, &TagTreeModel::on_treemodel_updated));
	model.signal_node_removed.connect(sigc::mem_fun(*this, &TagTreeModel::on_treemodel_removed));
}

TagTreeModel::~TagTreeModel() {
}

Glib::RefPtr<TagTreeModel> TagTreeModel::create(TaskManager& manager)
{
	return Glib::RefPtr<TagTreeModel>(new TagTreeModel(manager));
}

void TagTreeModel::set_value_impl(const iterator& row, int column, const Glib::ValueBase& value) {
	// TODO (for editing from TreeView)
}

Gtk::TreeModelFlags TagTreeModel::get_flags_vfunc() const {
	return Gtk::TreeModelFlags() & Gtk::TREE_MODEL_LIST_ONLY;
}

int TagTreeModel::get_n_columns_vfunc() const {
	return 2;
}

GType TagTreeModel::get_column_type_vfunc(int index) const {
	using namespace Gtk;
	switch(index) {
		case 0: return GTK_TYPE_STRING; break;
		case 1: return GTK_TYPE_INT; break;
	}
	return GTK_TYPE_OBJECT;
}

bool TagTreeModel::iter_next_vfunc(const iterator& iter, iterator& iter_next) const {
	if (!iter_is_valid(iter)) {
		iter_next = iterator();
		return false;
	}
	const GtkTreeIter* gtk_iter = iter.gobj();
	if (!gtk_iter || !gtk_iter->user_data) { return false; }

    TagNode* node = static_cast<TagNode*>(gtk_iter->user_data);
	GtkTreeIter* gtk_iter_next = iter_next.gobj();
	clearIter(gtk_iter_next);

    try {
        gtk_iter_next->user_data = &(model.get_next_node(*node));
    } catch(TagModel::InvalidNode&) {
		iter_next = iterator();
        return false;
    }
	iter_next.set_stamp(stamp);
	return true;
}

bool TagTreeModel::iter_children_vfunc(const iterator& parent, iterator& iter) const {
	// the first child of root
	GtkTreeIter* gtk_iter = iter.gobj();
	if (!parent->gobj()) {
		iter.set_stamp(stamp);
		clearIter(gtk_iter);
        gtk_iter->user_data = model.get_tags().front();
        return true;
    }

	const GtkTreeIter* gtk_parent_iter = parent.gobj();
	// invalid parent
    if (!gtk_parent_iter->user_data) {
        clearIter(gtk_iter);
        return false;
    }

    // children of ordinary node (non-root)
    TagNode* node = static_cast<TagNode*>(gtk_parent_iter->user_data);
    TagNodeVector& children(node->get_children());

	// no children
    if (children.empty()) {
        clearIter(gtk_iter);
        return false;
    }

	// first child
    iter.set_stamp(stamp);
	clearIter(gtk_iter);
    gtk_iter->user_data = children.front();
    return true;
}

bool TagTreeModel::iter_has_child_vfunc(const iterator& iter) const {
	TagNode* node = static_cast<TagNode*>(iter.gobj()->user_data);
    return node && !node->get_children().empty();
}

int TagTreeModel::iter_n_children_vfunc(const iterator& iter) const {
	if (!iter_is_valid(iter)) {
		return 0;
	}
	TagNode* node = static_cast<TagNode*>(iter.gobj()->user_data);
	return (node ? (int)node->get_children().size() : 0);
}

bool TagTreeModel::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const {
	if (!iter_is_valid(parent)) {
		iter = iterator();
		return false;
	}
	
	// TODO: fix the case when "static_cast<TagNode*>(parent->user_data)" is 0
	TagNodeVector& nodes( (!parent.gobj()) ? model.get_tags() :
        static_cast<TagNode*>(parent.gobj()->user_data)->get_children());

	GtkTreeIter* gtk_iter = iter.gobj();
	clearIter(gtk_iter);
    if ((int)nodes.size() < n) { return false; }

    TagNodeVector::iterator it = nodes.begin();
    std::advance(it, n);

    iter.set_stamp(stamp);
    gtk_iter->user_data = *it;
    return true;
}

bool TagTreeModel::iter_parent_vfunc(const iterator& child, iterator& iter) const {
	if (!iter_is_valid(iter)) {
		iter = iterator();
		return false;
	}

	GtkTreeIter* gtk_iter = iter.gobj();
	const GtkTreeIter* gtk_child_iter = child.gobj();
	clearIter(gtk_iter);
	if (gtk_child_iter && gtk_child_iter->user_data) {
        TagNode* node = static_cast<TagNode*>(gtk_child_iter->user_data);
        TagNode* parent = node->get_parent();
        
        if (parent) {
            iter.set_stamp(stamp);
            gtk_iter->user_data = parent;
            return true;
        }
    }
    return false;
}

void TagTreeModel::ref_node_vfunc(const iterator& iter) const {}

void TagTreeModel::unref_node_vfunc(const iterator& iter) const {}

Gtk::TreeModel::Path TagTreeModel::get_path_vfunc(const iterator& iter) const {
	const GtkTreeIter* it = iter.get_gobject_if_not_end();

    if (!it || !it->user_data) {
        return TreeModel::Path();
    }

	// convert TagModel iterator -> TagModel path -> TreeModel path
    TagNode* node = static_cast<TagNode*>(it->user_data);
    TagModel::Path path(model.get_path(*node));

    TreeModel::Path treePath;
    convert_path_tagmodel_treemodel(path, treePath);
    return treePath;
}

bool TagTreeModel::get_iter_vfunc(const TreeModel::Path& path, iterator& iter) const {
	if (path.size() < 1) {
		iter = iterator();
		return false;
	}

	TagModel::Path modelPath;
    // convert TreeModel path -> TagModel path
	modelPath.assign(path.begin(), path.end());
    
	GtkTreeIter* gtk_iter = iter.gobj();
	clearIter(gtk_iter);
	
    try {
        gtk_iter->user_data = &(model.get_node(modelPath));
    } catch(TagModel::InvalidPath&) {
		gtk_iter->user_data = 0;
        return false;
    }
	iter.set_stamp(stamp);
	return true;
}

void TagTreeModel::get_value_vfunc(const iterator& iter, int column, Glib::ValueBase& value) const {
	if (!iter_is_valid(iter)) { return; }
	const GtkTreeIter* it = iter.get_gobject_if_not_end();
	if (!it || !it->user_data) { return; }

	//check iter valitidy
    TagNode* node = static_cast<TagNode*>(it->user_data);

    switch(column) {
    case 0: // id
        value.init(G_TYPE_INT);
		g_value_set_int(value.gobj(), node->get_item().id);
        break;
    case 1: // name
        value.init(G_TYPE_STRING);
        g_value_set_string(value.gobj(), node->get_item().name.c_str());
        break;
	}
}

bool TagTreeModel::iter_is_valid(const iterator& iter) const {
	return ((iter.get_stamp() == stamp) ? Gtk::TreeModel::iter_is_valid(iter) : false);
}

void TagTreeModel::refresh() {
	model.refresh();
}

void TagTreeModel::clearIter(GtkTreeIter* iter) const {
	if (iter) {
		iter->stamp = 0;
		iter->user_data = iter->user_data2 = iter->user_data3 = 0;
	}
}

void TagTreeModel::convert_path_tagmodel_treemodel(const TagModel::Path& mpath, TreeModel::Path& tpath) const {
	TagModel::Path::const_iterator it;
	for (it = mpath.begin(); it != mpath.end(); ++it) {
        tpath.push_back(*it);
    }
}

void TagTreeModel::convert_path_treemodel_tagmodel(const TreeModel::Path& tpath, TagModel::Path& mpath) const {
	TreeModel::Path::const_iterator it;
	for (it = tpath.begin(); it != tpath.end(); ++it) {
        mpath.push_back(*it);
    }
}

void TagTreeModel::on_treemodel_inserted(TagNode& node, TagModel::Path& path) {
	// TODO: avoid duplicating code
	TreeModel::Path treePath;
	convert_path_tagmodel_treemodel(path, treePath);

    GtkTreeIter iter;
	clearIter(&iter);
	++stamp;
    iter.stamp = stamp;
    iter.user_data = &node;
	TreeModel::iterator treeIter(gobj(), &iter);
	row_inserted(treePath, treeIter);
}
void TagTreeModel::on_treemodel_updated(TagNode& node, TagModel::Path& path) {
	TreeModel::Path treePath;
	convert_path_tagmodel_treemodel(path, treePath);
	//iterator treeIter = get_iter(treePath);
	//row_changed(treePath, treeIter);

	GtkTreeIter iter;
	clearIter(&iter);
    ++stamp;
	iter.stamp = stamp;
    iter.user_data = &node;
	row_changed(treePath, TreeModel::iterator(gobj(), &iter));
}
void TagTreeModel::on_treemodel_removed(TagNode& node, TagModel::Path& path) {
	TreeModel::Path treePath;
	convert_path_tagmodel_treemodel(path, treePath);
	row_deleted(treePath);
}

// ----- class TagTreeModel_Class --------------------

const Glib::Class& TagTreeModel_Class::init()
{
    if (!gtype_)
    {
        class_init_func_ = &TagTreeModel_Class::class_init_function;

        const GTypeInfo derived_info = 
        {
            sizeof(GObjectClass),
            0,
            0,
            class_init_func_,
            0,
            0,
            sizeof(GObject),
            0,
            0,
            0,
        };
        
        gtype_ = g_type_register_static(G_TYPE_OBJECT, "GeToDo_TagTreeModel",
            &derived_info, GTypeFlags(0));

		Gtk::TreeModel::add_interface(get_type());
    }

    return *this;
}

void TagTreeModel_Class::class_init_function(void* g_class, void* class_data) {}

} // namespace getodo
