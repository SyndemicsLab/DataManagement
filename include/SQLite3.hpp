#ifndef SQLITE3_HPP
#define SQLITE3_HPP

#include <string>

/// @brief Namespace containing the Data objects belonging to the Simulation
namespace Data {
    /// @brief Object that stores the contents of a SQL table after it has been
    /// queried.
    class SQLTable {
    public:
        SQLTable() {}
        ~SQLTable();

        int nRows;
        int nCols;
        char **data;
        std::string name;
    };

    /// @brief Object that facilitates interfacing between the model and SQLite3
    /// databases.
    class Database;
} // namespace Data
#endif // SQLITE3_HPP
