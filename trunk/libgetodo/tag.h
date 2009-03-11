// $Id$
//
// C++ Interface: Tag
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
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
 */
class TagPersistence {
	sqlite3_connection* conn;
public:
	/** A constructor for loading new tags.
	 * \param conn A database connection. If it is not alright
	 * other methods will throw GetodoError exceptions.
	 */
	TagPersistence(sqlite3_connection* conn);
	virtual ~TagPersistence();

	/** Insert a new tag into the database.
	 * The tag is assumed to have no persistent id yet (eg. it was newly
	 * created), so a new id is assigned. It is both set to the given
	 * tag and returned.
	 * 
	 * Throws a GetodoError if the database connection is broken.
	 *
	 * \return id id assigned by the database.
	 */
	id_t insert(Tag& tag);
	
	/** Update an existing tag.
	 * The tag is assumend to have a valid persistent id, otherwise
	 * an exception is thrown.
	 * If there's no such a tag in the database to update nothing happens.
	 *
	 * Throws a GetodoError if the database connection is broken.
	 * Throws a std::invalid_argument, if the id is not valid.
	 */
	void update(const Tag& tag);
	
	/** Load a tag from the database. */
	Tag& load(id_t id);
	
	/** Delete a tag from the database. */
	void erase(id_t id);
};

} // namespace getodo

#endif // LIBGETODO_TAG_H
