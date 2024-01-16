#include "SQLite3.hpp"

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <unordered_map>

namespace Data {
    class Database {
    public:
        /// @brief Constructor for the class that handles SQLite3 interfacing
        /// @param database The path to a SQLite3 database
        Database(std::string database);
        ~Database();

        /// @brief
        /// @param
        SQLTable readTable(std::string query);

    private:
        // pointer for the sqlite3 connection
        sqlite3 *db;
    };

    SQLTable::~SQLTable() { sqlite3_free_table(this->data); }

    Database::Database(std::string database) {
        // evaluates as true if an error is raised
        if (sqlite3_open_v2(database.c_str(), &this->db, SQLITE_OPEN_READONLY,
                            NULL)) {
            // add error handling here
            return;
        }
    }

    Database::~Database() { sqlite3_close(this->db); }

    SQLTable Database::readTable(std::string query) {
        SQLTable toReturn;

        // evaluates as true if an error is raised
        if (sqlite3_get_table(this->db, query.c_str(), &toReturn.data,
                              &toReturn.nRows, &toReturn.nCols, NULL)) {
            // add error handling here
        }

        return toReturn;
    }
} // namespace Data
