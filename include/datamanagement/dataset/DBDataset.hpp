#ifndef DBDATASET_HPP_
#define DBDATASET_HPP_

#include <Dataset.hpp>
#include <filesystem>
#include <fstream>
#include <sqlite3.h>
#include <string>
#include <vector>

namespace datamanagement {
    using Row = std::vector<std::string>;
    using Table = std::vector<Row>;
    class DBDataset : public virtual Dataset {
    private:
        /* data */
        sqlite3 *db;
        std::string dbf = "";
        bool db_isopen = false;

        // Generalized Callback function used to return a vector of string (each
        // string is a column)
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            Table *vecPtr = (Table *)storage;
            Row row;
            for (int i = 0; i < count; ++i) {
                row.push_back(std::string(data[i]));
            }
            vecPtr->push_back(row);
            return 0;
        }

        // Be Very Careful! void* is expected to be a Table*
        // because sqlite and C
        int ExecuteQuery(std::string const query,
                         int (*callback_func)(void *, int, char **, char **),
                         void *data) const {
            StartTransaction();
            char *error_message;
            int rc = sqlite3_exec(db, query.c_str(), callback_func, data,
                                  &error_message);
            EndTransaction();
            Table *vecPtr = (Table *)data;
            Row row;
            // If there is an error, return the error message in the first index
            // of the data vector
            if (rc != SQLITE_OK) {
                row.push_back(CleanErrorMessages(error_message));
                vecPtr->push_back(row);
                return rc;
            }

            if (data == nullptr) {
                row.push_back("");
                vecPtr->push_back(row);
            }
            return rc;
        }

        /// @brief Function to Build a Prepared Statement. NOTE: User must call
        /// sqlite3_finalize later on to free the memory allocated here.
        /// @param query String to add with the binding spaces marked
        sqlite3_stmt *BuildTextPreparedStatement(std::string query) const {
            sqlite3_stmt *stmt;
            sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
            return stmt;
        }

        int
        BindTextToPreparedStatement(sqlite3_stmt *stmt,
                                    std::vector<std::string> bindings) const {
            if (stmt == nullptr) {
                return -1;
            }
            for (int i = 0; i < bindings.size(); ++i) {
                sqlite3_bind_text(stmt, i + 1, bindings[i].c_str(), -1,
                                  SQLITE_TRANSIENT);
            }
            return 0;
        }

        int StepAndResetStatement(sqlite3_stmt *stmt) const {
            if (stmt == nullptr) {
                return -1;
            }
            sqlite3_step(stmt);
            sqlite3_clear_bindings(stmt);
            return sqlite3_reset(stmt);
        }

        /// @brief THIS FUNTION MUST BE CALLED IF YOU CREATE A PREPARED
        /// STATEMENT THROUGH BuildTextPreparedStatement
        /// @param stmt
        int FinalizePreparedStatement(sqlite3_stmt *stmt) const {
            return sqlite3_finalize(stmt);
        }

        /// @brief I HATE SQLITE SOMETIMES
        /// @param message The char * to free
        /// @return The string copy of the error message that has scope
        std::string CleanErrorMessages(char *message) const {
            std::string usable_error = std::string(message);
            sqlite3_free(message);
            return usable_error;
        }

    public:
        DBDataset() {}
        ~DBDataset() { CloseConnection(); }

        void ConnectToDatabase(const std::string &path) {
            dbf = path;
            int rc = sqlite3_open(path.c_str(), &db);
            db_isopen = (rc == SQLITE_OK) ? true : false;
        }
        int CloseConnection() {
            int rc = 0;
            if (db_isopen) {
                rc = sqlite3_close(db);
                db_isopen = (rc == SQLITE_OK) ? false : true;
            }
            return rc;
        }

        int SaveDatabase(std::string const &outfile) {
            if (db_isopen) {
                CloseConnection();
            }
            if (std::filesystem::exists(GetDBFileName())) {
                std::ifstream src(GetDBFileName(), std::ios::binary);
                std::ofstream dest(outfile, std::ios::binary);
                dest << src.rdbuf();
                src.close();
                dest.close();
                return 0;
            } else {
                return -1;
            }
        }

        int StartTransaction() const {
            char *error_message;
            return sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL,
                                &error_message);
        }

        int EndTransaction() const {
            char *error_message;
            return sqlite3_exec(db, "END TRANSACTION", NULL, NULL,
                                &error_message);
        }
        std::string GetDBFileName() const { return dbf; }

        int Create(std::string const query, Table &data) const {
            return ExecuteQuery(query, NULL, &data);
        }
        int Select(std::string const query, Table &data) const {
            return ExecuteQuery(query, callback, &data);
        }
        int Update(std::string const query, Table &data) const {
            return ExecuteQuery(query, callback, &data);
        }
        int Delete(std::string const query, Table &data) const {
            return ExecuteQuery(query, callback, &data);
        }
        int SelectCustomCallback(std::string const query,
                                 int (*callback_func)(void *, int, char **,
                                                      char **),
                                 void *data, std::string &error) const {
            char *error_message;
            StartTransaction();
            int rc = sqlite3_exec(db, query.c_str(), callback_func, data,
                                  &error_message);
            EndTransaction();
            if (rc != SQLITE_OK) {
                error = CleanErrorMessages(error_message);
            }
            return rc;
        }

        int GetRun() const override {}
        int GetTarget() const override {}
    };
} // namespace datamanagement

#endif