////////////////////////////////////////////////////////////////////////////////
// File: db_source.hpp                                                        //
// Project: source                                                            //
// Created Date: Th Feb 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
// -----                                                                      //
// HISTORY:                                                                   //
// Date      	By	Comments                                                  //
// ----------	---	--------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATAMANAGEMENT_SOURCE_DBDATASOURCE_HPP_
#define DATAMANAGEMENT_SOURCE_DBDATASOURCE_HPP_

#include <SQLiteCpp/SQLiteCpp.h>
#include <any>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace datamanagement::source {
using BindingVariant = std::variant<int, double, std::string>;
class DBSource {
public:
    DBSource() {}
    ~DBSource() = default;

    // Move Constructor
    DBSource(DBSource &&old) = default;
    DBSource &operator=(DBSource &&) = default;

    void SetDatabasePath(const std::string &p) { _path = p; }

    std::string GetDatabaseFileName() const {
        std::filesystem::path p = _path;
        return p.stem();
    }

    void
    Select(const std::string &query,
           std::function<void(std::any &storage, const SQLite::Statement &stmt)>
               callback,
           std::any &storage,
           const std::unordered_map<int, BindingVariant> &bindings = {}) {
        try {
            SQLite::Database db(_path,
                                SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            SQLite::Statement stmt(db, query);

            for (const auto &[index, value] : bindings) {
                if (value.index() == 0) {
                    stmt.bind(index, std::get<int>(value));
                } else if (value.index() == 1) {
                    stmt.bind(index, std::get<double>(value));
                } else {
                    stmt.bind(index, std::get<std::string>(value));
                }
            }

            SQLite::Transaction transaction(db);

            while (stmt.executeStep()) {
                callback(storage, stmt);
            }

            transaction.commit();
        } catch (const std::exception &e) {
            throw std::runtime_error("Error executing query: " + query + "\n" +
                                     e.what());
        }
    }

    void BatchExecute(const std::string &query,
                      const std::vector<std::unordered_map<int, BindingVariant>>
                          &bindings_batch = {}) {
        try {
            SQLite::Database db(_path,
                                SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            SQLite::Transaction transaction(db);
            SQLite::Statement stmt(db, query);

            for (auto &bindings : bindings_batch) {
                for (const auto &[index, value] : bindings) {
                    if (value.index() == 0) {
                        stmt.bind(index, std::get<int>(value));
                    } else if (value.index() == 1) {
                        stmt.bind(index, std::get<double>(value));
                    } else {
                        stmt.bind(index, std::get<std::string>(value));
                    }
                }
                stmt.exec();
                stmt.reset();
            }
            transaction.commit();

        } catch (const std::exception &e) {
            throw std::runtime_error("Error executing query: " + query + "\n" +
                                     e.what());
        }
    }

private:
    std::string _path = "";
};
} // namespace datamanagement::source

#endif // DATAMANAGEMENT_SOURCE_DBDATASOURCE_HPP_