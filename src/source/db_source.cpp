////////////////////////////////////////////////////////////////////////////////
// File: db_source.cpp                                                        //
// Project: source                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-01                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <datamanagement/source/db_source.hpp>

namespace datamanagement {
namespace source {

void DBSource::Select(
    const std::string &query,
    std::function<void(std::any &storage, const SQLite::Statement &stmt)>
        callback,
    std::any &storage,
    const std::unordered_map<int, BindingVariant> &bindings) {
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

void DBSource::BatchExecute(
    const std::string &query,
    const std::vector<std::unordered_map<int, BindingVariant>>
        &bindings_batch) {
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
} // namespace source
} // namespace datamanagement