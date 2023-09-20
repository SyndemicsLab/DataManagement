#ifndef SQLITE3_HPP
#define SQLITE3_HPP

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <unordered_map>

/// @brief Namespace containing the Data objects belonging to the Simulation
namespace Data {
    /// @brief Object that stores the contents of a SQL table after it has been
    /// queried.
    class SQLTable {
    public:
        SQLTable() {}
        ~SQLTable() { sqlite3_free_table(this->data); }

        int nRows;
        int nCols;
        char **data;
        std::string name;
    };

    /// @brief Object that facilitates interfacing between the model and SQLite3
    /// databases.
    class Database {
    public:
        /// @brief Constructor for the class that handles SQLite3 interfacing
        /// @param database The path to a SQLite3 database
        Database(std::string database);
        ~Database() { sqlite3_close(this->db); }

        /// @brief
        /// @param
        SQLTable readTable(std::string query);

    private:
        // pointer for the sqlite3 connection
        sqlite3 *db;
    };
} // namespace Data
#endif // SQLITE3_HPP
