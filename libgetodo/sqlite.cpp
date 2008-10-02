// $Id: $

#include "common.h"

namespace getodo {

std::string sqliteEscapeString(const std::string& input) {
	return boost::algorithm::replace_all_copy(input, "'", "''");
}

std::string sqliteUnescapeString(const std::string& input) {
	return boost::algorithm::replace_all_copy(input, "''", "'");
}

} // namespace getodo
