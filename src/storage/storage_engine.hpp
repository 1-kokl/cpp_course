#ifndef STORAGE_ENGINE_HPP
#define STORAGE_ENGINE_HPP

#include "database.hpp"

bool create_db(const char* dbname);
bool drop_db(const char* dbname);
Database* use_db(const char* dbname);
Database* current_db();

#endif
