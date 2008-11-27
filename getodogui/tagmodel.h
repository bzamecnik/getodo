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

//class TagManager : public sigc::trackable
//{
//// A container of Tags that signals the operations with Tags.
//// Use TagManager interface for modyfying the data.
//private:
//	typedef std::map<id_t, Tag*> tags_t;
//	tags_t tags;
//
//public:
//	TagManager();
//	virtual ~TagManager();
//
//	void insertTag(Tag* tag);
//	bool hasTag(id_t tagId);
//	Tag* getTag(id_t tagId);
//	void updateTag(Tag* tag);
//	void removeTag(id_t tagId);
//	std::list<Tag*> getTagsList();
//	
//	sigc::signal1<void, Tag&> signal_tag_inserted;
//	sigc::signal1<void, Tag&> signal_tag_updated;
//	sigc::signal1<void, Tag&> signal_tag_removed;
//};

class TagNode;
typedef std::vector<TagNode*> TagNodeVector;

/** %Tag node.
 * A wrapper for Tag objects in TagModel.
 */
class TagNode
{
public:
    TagNode(Tag& Tag) : children(0), parent(0), tag(Tag)
    { }
    virtual ~TagNode();

    bool has_children(void) {
        return !children.empty();
    }
	TagNodeVector& get_children(void) { return children; }
	TagNode* get_parent(void) { return parent; }
    Tag& get_item(void) { return tag; }
    const Tag& get_item(void) const { return tag; }
protected:
    TagNodeVector children;
    TagNode* parent;
private:
	Tag& tag;
};

/** %Tag model.
 * TagModel acts as a wrapper over TagManager that holds its tree structure.
 * It proxies signals from TagManager to TagTreeModel.
 * - for now TagModel has a flat structure
 * - TODO: make it have a tree structure
 * - maybe merge TagModel and TagTreeModel
*/
class TagModel : public sigc::trackable {
public:
    typedef std::deque<int> Path;
	typedef std::map<id_t, TagNode*> TagNodeMap;

    struct InvalidNode { }; // an exception
    struct InvalidPath { }; // an exception

	TagModel(TaskManager& manager);
    virtual ~TagModel();

	TagNodeVector& get_tags() const;
    TagNode& get_next_node(TagNode& node) const;
    TagNode& get_node(Path& path) const;
    Path get_path(TagNode& node) const;

	void refresh(void);
	void clear(void);

	// signals for TagTreeModel
    sigc::signal2<void, TagNode&, Path&> signal_node_inserted;
    sigc::signal2<void, TagNode&, Path&> signal_node_updated;
    sigc::signal2<void, TagNode&, Path&> signal_node_removed;
private:
	TaskManager& manager;
	TagNodeVector tagNodes; // children of root
	TagNodeMap tagNodeMap;

	void remove(Tag& tag);
	void insert(Tag& tag);

	// called by underlying TagManager
	void on_tag_inserted(Tag&);
	void on_tag_updated(Tag&);
	void on_tag_removed(Tag&);
};

/** %Tag TreeModel.
 * A custom Gtk::TreeModel implementation for using TagManager from TreeView.
 */
class TagTreeModel : public Glib::Object, public Gtk::TreeModel {
// 
public:
	struct ModelColumns : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<int> id;
		Gtk::TreeModelColumn<Glib::ustring> name;

		ModelColumns() {
			add(id), add(name);
		}
	};
	const ModelColumns columns;

	~TagTreeModel();

	static Glib::RefPtr<TagTreeModel> create(TaskManager& manager);

	void clear();
protected:
	TagTreeModel(TaskManager& manager);

    virtual void set_value_impl(const iterator& row, int column, const Glib::ValueBase& value);
    //virtual void get_value_impl(const iterator& row, int column, Glib::ValueBase& value) const;

	//  Gtk::TreeModel stuff
    virtual GType get_column_type_vfunc(int index) const;
	virtual Gtk::TreeModelFlags get_flags_vfunc() const;
    virtual bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;
    virtual bool iter_children_vfunc(const iterator& parent, iterator& itert) const;
    virtual bool iter_has_child_vfunc(const iterator& iter) const;
    virtual int iter_n_children_vfunc(const iterator& iter) const;
	virtual int get_n_columns_vfunc() const;
    virtual bool iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const;
    virtual bool iter_parent_vfunc(const iterator& child, iterator& iter) const;
    virtual void ref_node_vfunc(const iterator& iter) const;
    virtual void unref_node_vfunc(const iterator& iter) const;
    virtual TreeModel::Path get_path_vfunc(const iterator& iter) const;
    virtual bool get_iter_vfunc(const Path& path, iterator& iter) const;
    virtual void get_value_vfunc(const iterator& iter, int column, Glib::ValueBase& value) const;
	virtual bool iter_is_valid(const iterator& iter) const; // fix stamp validating
private:	
	TagModel model;
	int stamp;
	
	// GTK+ class stuff
    friend class TagTreeModel_Class;
    static TagTreeModel_Class tagtreemodel_class_;

	void refresh();
    void clearIter(GtkTreeIter* iter) const;

	void convert_path_tagmodel_treemodel(const TagModel::Path& mpath, TreeModel::Path& tpath) const;
	void convert_path_treemodel_tagmodel(const TreeModel::Path& tpath, TagModel::Path& mpath) const;

	// called from underlying TagModel
	void on_treemodel_inserted(TagNode& node, TagModel::Path& path);
    void on_treemodel_updated(TagNode& node, TagModel::Path& path);
    void on_treemodel_removed(TagNode& node, TagModel::Path& path);
}; // class TagTreeModel

/** TagTreeModel class for GTK+.
 */
class TagTreeModel_Class : public Glib::Class
{
public:
    struct TagTreeModelClass
    {
        GObjectClass parent_class;
    };

    friend class TagTreeModel;

    const Glib::Class& init();
    static void class_init_function(void* g_class, void* class_data);
}; // class TagTreeModel_Class

} // namespace getodo

#endif // TAG_MODEL_H
