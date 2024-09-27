#include "DataManager.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <vector>

namespace datamanagement {
    class DataManager::Database {
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
            char *error_message;
            int rc = sqlite3_exec(db, query.c_str(), callback_func, data,
                                  &error_message);
            if (rc != SQLITE_OK) {
                return rc;
            }
            // If there is an error, return the error message in the first index
            // of the data vector
            if (data == nullptr) {
                return rc;
            }
            Table *vecPtr = (Table *)data;
            Row row;
            if (rc != SQLITE_OK) {
                row.push_back(std::string(error_message));
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

    public:
        Database() {}

        int ConnectToDatabase(std::string const &dbfile) {
            dbf = dbfile;
            int rc = sqlite3_open(dbfile.c_str(), &db);
            db_isopen = (rc == SQLITE_OK) ? true : false;
            return rc;
        }

        // Must always close the DB and delete the temp db file when the Manager
        // ends
        ~Database() {
            if (db_isopen) {
                sqlite3_close(db);
            }
        }

        // CRUD Logical Wrappers
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

        // I Don't Like how unsecure this feels, but we need the ability to
        // manipulate the callback
        int SelectCustomCallback(std::string const query,
                                 int (*callback_func)(void *, int, char **,
                                                      char **),
                                 void *data, std::string &error) const {
            char *error_message;
            int rc = sqlite3_exec(db, query.c_str(), callback_func, data,
                                  &error_message);
            if (rc != SQLITE_OK) {
                error = std::string(error_message);
            }
            return rc;
        }

        int SaveDatabase(std::string const &outfile) {
            if (db) {
                sqlite3_close(db);
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
        int AddCSVTable(std::string const &file) const {
            std::ifstream csv;
            Table data;
            csv.open(file, std::ios::in);
            if (!csv) {
                return false;
            }
            std::string table = std::filesystem::path(file).stem();
            std::string createquery =
                "CREATE TABLE IF NOT EXISTS " + table + "(";
            std::string insertquery = "INSERT INTO '" + table + "' VALUES (";
            std::string line, word;
            std::getline(csv, line);
            std::stringstream s(line);

            // Talk about insecure practices and SQL Injections
            while (std::getline(s, word, ',')) {
                createquery += (word + " NOT NULL,");
                insertquery += "?,";
            }
            createquery.pop_back(); // remove last comma
            createquery += ");";
            insertquery.pop_back();
            insertquery += ");";
            int rc = (SQLITE_OK == Create(createquery, data)) ? SQLITE_OK
                                                              : SQLITE_ERROR;

            StartTransaction();
            sqlite3_stmt *stmt = BuildTextPreparedStatement(insertquery);

            // Grab a Row
            while (std::getline(csv, line)) {
                Row row = {};
                std::stringstream s(line);
                // Grab a Entry in the Row
                while (std::getline(s, word, ',')) {
                    row.push_back(word);
                }
                BindTextToPreparedStatement(stmt, row);
                StepAndResetStatement(stmt);
            }
            FinalizePreparedStatement(stmt);
            EndTransaction();
            csv.close();
            return rc;
        }

        int WriteTableToCSV(std::string const &filepath, std::string tablename,
                            std::string column_names) const {
            std::ofstream csv;
            csv.open(filepath, std::ofstream::out);
            if (!csv) {
                return false;
            }
            csv << column_names << std::endl;

            Table data;
            std::stringstream query;
            query << "SELECT " << column_names << " FROM " << tablename;
            Select(query.str(), data);
            for (auto &row : data) {
                for (auto &val : row) {
                    csv << val;
                }
                csv << std::endl;
            }
            csv.close();
            return 0;
        }

        std::string GetDBFileName() const { return dbf; }
    };

    class DataManager::Config {
    private:
        boost::property_tree::ptree ptree;
        bool tree_loaded = false;

    public:
        Config() {}
        ~Config() = default;

        int LoadConfig(std::string const &filepath) {
            read_ini(filepath, this->ptree);
            tree_loaded = true;
            return 0;
        }

        int GetFromConfig(std::string const key, std::string &data) const {
            if (!tree_loaded) {
                return -1;
            }
            try {
                data = ptree.get<std::string>(key);
            } catch (const std::exception &e) {
                // log bad cast
                return -1;
            }
            return 0;
        }

        int GetConfigSectionCategories(std::string const section,
                                       std::vector<std::string> &data) const {
            if (!tree_loaded) {
                return -1;
            }
            boost::property_tree::ptree subTree =
                this->ptree.get_child(section);
            std::vector<std::string> keyList;

            for (auto &key : subTree) {
                keyList.push_back(key.first);
            }
            data = keyList;
            return 0;
        }
    };

    int DataManager::LoadConfig(std::string const &filepath) {
        return pImplCF->LoadConfig(filepath);
    }

    int DataManager::GetFromConfig(std::string const key,
                                   std::string &data) const {
        return pImplCF->GetFromConfig(key, data);
    }

    int DataManager::GetConfigSectionCategories(
        std::string const section, std::vector<std::string> &data) const {
        return pImplCF->GetConfigSectionCategories(section, data);
    }

    int DataManager::AddCSVTable(std::string const &file) const {
        return pImplDB->AddCSVTable(file);
    }
    int DataManager::WriteTableToCSV(std::string const &filepath,
                                     std::string tablename,
                                     std::string column_names) const {
        return pImplDB->WriteTableToCSV(filepath, tablename, column_names);
    }
    int DataManager::Create(std::string const query, Table &data) const {
        return pImplDB->Create(query, data);
    }
    int DataManager::Select(std::string const query, Table &data) const {
        return pImplDB->Select(query, data);
    }
    int DataManager::Update(std::string const query, Table &data) const {
        return pImplDB->Update(query, data);
    }
    int DataManager::Delete(std::string const query, Table &data) const {
        return pImplDB->Delete(query, data);
    }

    int DataManager::SelectCustomCallback(
        std::string const query,
        int (*callback_func)(void *, int, char **, char **), void *data,
        std::string &error) const {
        return pImplDB->SelectCustomCallback(query, callback_func, data, error);
    }

    int DataManager::SaveDatabase(std::string const &outfile) {
        return pImplDB->SaveDatabase(outfile);
    }

    int DataManager::StartTransaction() const {
        return pImplDB->StartTransaction();
    }
    int DataManager::EndTransaction() const {
        return pImplDB->EndTransaction();
    }

    std::string DataManager::GetDBFileName() const {
        return pImplDB->GetDBFileName();
    }

    int DataManager::ConnectToDatabase(std::string const &dbfile) {
        return pImplDB->ConnectToDatabase(dbfile);
    }

    DataManager::DataManager() {
        pImplDB = std::make_unique<Database>();
        pImplCF = std::make_unique<Config>();
    }
    DataManager::~DataManager() = default;

    DataManager::DataManager(DataManager &&) noexcept = default;
    DataManager &DataManager::operator=(DataManager &&) noexcept = default;

} // namespace datamanagement