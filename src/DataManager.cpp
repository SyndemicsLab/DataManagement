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
            // If there is an error, return the error message in the first index
            // of the data vector
            Table *vecPtr = (Table *)data;
            Row row;
            if (rc != SQLITE_OK) {
                row.push_back(std::string(error_message));
                vecPtr->push_back(row);
            }
            return rc;
        }

    public:
        // This works because we don't care about threadsafe currently. We
        // assume a single DB for the project
        Database(/* args */) { sqlite3_open("temp.db", &db); }

        // Must always close the DB and delete the temp db file when the Manager
        // ends
        ~Database() {
            sqlite3_close(db);
            std::filesystem::remove("temp.db");
        }

        // CRUD Logical Wrappers
        int Create(std::string const query, Table &data) const {
            return ExecuteQuery(query, NULL, 0);
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
            if (std::filesystem::exists("temp.db")) {
                std::ifstream src("temp.db", std::ios::binary);
                std::ofstream dest(outfile, std::ios::binary);
                dest << src.rdbuf();
                src.close();
                dest.close();
                return 0;
            } else {
                return -1;
            }
        }
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

    DataManager::DataManager() {
        pImplDB = std::make_unique<Database>();
        pImplCF = std::make_unique<Config>();
    }

    DataManager::~DataManager() = default;

    int DataManager::AddCSVTable(std::string const &file) const {
        std::ifstream csv;
        Table data;
        csv.open(file, std::ios::in);
        if (!csv) {
            return false;
        }
        std::string table = std::filesystem::path(file).stem();
        std::string sql = "CREATE TABLE IF NOT EXISTS " + table + "(";
        std::string line, word, temp;
        std::vector<std::string> headers;
        std::getline(csv, line);
        std::stringstream s(line);

        // Talk about insecure practices and SQL Injections
        while (std::getline(s, word, ',')) {
            headers.push_back(word);
            sql += (word + " NOT NULL,");
        }
        sql.pop_back(); // remove last comma
        sql += ");";
        int rc = (SQLITE_OK == DataManager::Update(sql, data)) ? SQLITE_OK
                                                               : SQLITE_ERROR;

        sql.clear();
        sql = "BEGIN TRANSACTION;";
        rc = (SQLITE_OK == DataManager::Update(sql, data)) ? rc : SQLITE_ERROR;

        // Talk about insecure practices and SQL Injections
        while (std::getline(csv, line)) {
            sql.clear();
            sql = "INSERT INTO " + this->quoter(table) + " VALUES (";
            std::stringstream s(line);
            while (std::getline(s, word, ',')) {
                sql += (this->quoter(word) + ",");
            }
            sql.pop_back(); // remove last comma
            sql += ");";
            rc = (SQLITE_OK == DataManager::Update(sql, data)) ? rc
                                                               : SQLITE_ERROR;
        }
        sql.clear();
        sql = "COMMIT;";
        rc = (SQLITE_OK == DataManager::Update(sql, data)) ? rc : SQLITE_ERROR;

        csv.close();
        return rc;
    }
    int DataManager::WriteTableToCSV(std::string const &filepath,
                                     std::string tablename,
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
        pImplDB->Select(query.str(), data);
        for (auto &row : data) {
            for (auto &val : row) {
                csv << val;
            }
            csv << std::endl;
        }
        csv.close();
        return 0;
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

} // namespace datamanagement