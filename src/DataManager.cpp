#include "DataManager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <vector>

namespace datamanagement {
    class DataManagerBase::DataManager {
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
        DataManager(/* args */) { sqlite3_open("temp.db", &db); }

        // Must always close the DB and delete the temp db file when the Manager
        // ends
        ~DataManager() {
            sqlite3_close(db);
            std::filesystem::remove("temp.db");
        }

        // CRUD Logical Wrappers
        int Create(std::string const query, Table data) const {
            return ExecuteQuery(query, NULL, 0);
        }
        int Select(std::string const query, Table data) const {
            return ExecuteQuery(query, callback, &data);
        }
        int Update(std::string const query, Table data) const {
            return ExecuteQuery(query, callback, &data);
        }
        int Delete(std::string const query, Table data) const {
            return ExecuteQuery(query, callback, &data);
        }
    };

    DataManagerBase::DataManagerBase() {
        pImplDM = std::make_unique<DataManager>();
    }

    DataManagerBase::~DataManagerBase() = default;

    int DataManagerBase::AddCSVTable(std::string const &file) const {
        std::ifstream csv;
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
        sql += ");";
        int rc = DataManagerBase::Create(sql, {});
        if (rc != SQLITE_OK) {
            // log error
            csv.close();
            return;
        }

        sql.clear();
        sql = "INSERT INTO " + table + "(";
        for (std::string &head : headers) {
            sql += (head + ",");
        }
        sql += ") VALUES ";

        // Talk about insecure practices and SQL Injections
        while (csv >> temp) {
            sql += "(";
            std::getline(csv, line);
            std::stringstream s(line);
            while (std::getline(s, word, ',')) {
                sql += (word + ",");
            }
            sql += ")";
        }
        rc = DataManagerBase::Update(sql, {});
        if (rc != SQLITE_OK) {
            // log error
            csv.close();
            return;
        }

        csv.close();
        return 0;
    }
    int DataManagerBase::Create(std::string const query, Table data) const {
        return pImplDM->Create(query, data);
    }
    int DataManagerBase::Select(std::string const query, Table data) const {
        return pImplDM->Select(query, data);
    }
    int DataManagerBase::Update(std::string const query, Table data) const {
        return pImplDM->Update(query, data);
    }
    int DataManagerBase::Delete(std::string const query, Table data) const {
        return pImplDM->Delete(query, data);
    }
} // namespace datamanagement