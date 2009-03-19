// $Id$
//
// Classes Tag, TagPersistence
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef LIBGETODO_TAG_H
#define LIBGETODO_TAG_H

#include "common.h"
#include <sstream>

using namespace sqlite3x;

namespace getodo {

/** %Tag.
 * Tag is a kind category. Multiple tags can be given to a task.
 */
class Tag {
public:
	/** Tag id. INVALID_ID, if not already in database. */
	id_t id;
	/** Tag name */
	std::string name;

	Tag();
	Tag(const Tag& t);
	Tag(std::string name);
	Tag(id_t tagId, std::string tagName);
	virtual ~Tag();

	/* Serialization. */
	std::string toString() const;
	/* Deserialization. */
	friend std::ostream& operator<< (std::ostream& o, const Tag& tag);

	/** Invalid identifier. A tag which hasn't been stored into database yet
	 * will have this id */
	static const id_t INVALID_ID = -1;
	/** Check if the id can belong to a tag which is already in the database. */
	static bool isValidId(id_t id);
	/** Check if the tag has such an id which says it has already been stored into
	 * the database.
	 */
	bool hasValidId() const;
};

/** %Tag persistence.
 * Object-relation mapping of Tag objects.
 *
 * There is a CRUD (Create, Read, Update, Delete) interface:
 * - insert()
 * - load()
 * - update()
 * - erase()
 */
class TagPersistence {
	boost::shared_ptr<sqlite3_connection> conn;
public:
	/** Constructor for loading new tags.
	 * \param conn A database connection. If it is not alright
	 * other methods will throw GetodoError exceptions.
	 */
	TagPersistence(boost::shared_ptr<sqlite3_connection> conn);
	virtual ~TagPersistence();

	/** Insert a new tag into the database.
	 * The tag is assumed to have no persistent id yet (eg. it was newly
	 * created), so a new id is assigned. It is both set to the given
	 * tag and returned.
	 * 
	 * \param tag tag to be inserted to database
	 *
	 * \throw GetodoError if the database connection is broken
	 * \throw GetodoError if the tag name is empty
	 *
	 * \return id id assigned by the database
	 */
	Tag insert(const Tag& tag);
	
	/** Update an existing tag.
	 * The tag is assumend to have a valid persistent id, otherwise
	 * an exception is thrown.
	 * If there's no such a tag in the database to update nothing happens.
	 *
	 * \param tag tag to be updated
	 *
	 * \throw GetodoError if the database connection is broken
	 * \throw std::invalid_argument, if the id is not valid
	 */
	void update(const Tag& tag);
	
	/** Load a tag from the database.
	 * Create a tag from using data from database.
	 *
	 * \param id id of tag to load
	 *
	 * \throw GetodoError if the database connection is broken
	 * \throw GetodoError if there is no such a tag
	 *
	 * \return tag loaded from database
	 */
	Tag load(id_t id);
	
	/** Delete a tag from the database.
	 * If there is no such a tag nothing happens.
	 *
	 * \throw GetodoError if the database connection is broken
	 * \param id id tag of tag to delete
	 */
	void erase(id_t id);
};

} // namespace getodo

#endif // LIBGETODO_TAG_H
