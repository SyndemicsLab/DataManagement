////////////////////////////////////////////////////////////////////////////////
// File: db_source.hpp                                                        //
// Project: source                                                            //
// Created Date: Th Feb 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-01                                                  //
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

#include <any>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <variant>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

namespace datamanagement::source {
using BindingVariant = std::variant<int, double, std::string>;
class DBSource {
public:
    DBSource() = default;
    ~DBSource() = default;

    // Move Constructor
    DBSource(DBSource &&old) = default;
    DBSource &operator=(DBSource &&) = default;

    inline void SetDatabasePath(const std::string &p) { _path = p; }

    inline std::string GetDatabaseFileName() const {
        std::filesystem::path p = _path;
        return p.stem();
    }

    void
    Select(const std::string &query,
           std::function<void(std::any &storage, const SQLite::Statement &stmt)>
               callback,
           std::any &storage,
           const std::unordered_map<int, BindingVariant> &bindings = {});

    void BatchExecute(const std::string &query,
                      const std::vector<std::unordered_map<int, BindingVariant>>
                          &bindings_batch = {});

private:
    std::string _path = "";
};
} // namespace datamanagement::source

#endif // DATAMANAGEMENT_SOURCE_DBDATASOURCE_HPP_