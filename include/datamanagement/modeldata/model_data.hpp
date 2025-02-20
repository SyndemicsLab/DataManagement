////////////////////////////////////////////////////////////////////////////////
// File: model_data.hpp                                                       //
// Project: modeldata                                                         //
// Created Date: Th Feb 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: Thu Feb 20 2025                                             //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
// -----                                                                      //
// HISTORY:                                                                   //
// Date      	By	Comments                                                  //
// ----------	---	--------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATAMANAGEMENT_MODELDATA_MODELDATA_HPP_
#define DATAMANAGEMENT_MODELDATA_MODELDATA_HPP_

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <datamanagement/source/config.hpp>
#include <datamanagement/source/csv_source.hpp>
#include <datamanagement/source/db_source.hpp>

namespace datamanagement {
class ModelData {
private:
    datamanagement::source::Config config;
    std::unordered_map<std::string, datamanagement::source::CSVSource>
        _csv_sources = {};
    std::unordered_map<std::string, datamanagement::source::DBSource>
        _db_sources = {};

public:
    ModelData(const std::string &cfgfile) : config(cfgfile) {}
    ~ModelData() = default;
    datamanagement::source::Config GetConfig() const { return config; }

    void AddSource(const std::string &path) {
        std::filesystem::path p = path;
        if (p.extension() == ".csv") {
            datamanagement::source::CSVSource s;
            _csv_sources[p.stem()] = std::move(s);
            _csv_sources[p.stem()].ConnectToFile(path);
        } else if (p.extension() == ".db") {
            datamanagement::source::DBSource s;
            _db_sources[p.stem()] = std::move(s);
            _db_sources[p.stem()].ConnectToDatabase(path);
        } else {
            // Not a valid source file
        }
    }

    std::vector<std::string> GetCSVSourceNames() const {
        std::vector<std::string> names = {};
        for (const auto &[k, v] : _csv_sources) {
            names.push_back(k);
        }
        return names;
    }
    std::vector<std::string> GetDBSourceNames() const {
        std::vector<std::string> names = {};
        for (const auto &[k, v] : _db_sources) {
            names.push_back(k);
        }
        return names;
    }

    datamanagement::source::CSVSource &GetCSVSource(const std::string &name) {
        return _csv_sources[name];
    }

    datamanagement::source::DBSource &GetDBSource(const std::string &name) {
        return _db_sources[name];
    }
};
} // namespace datamanagement

#endif // DATAMANAGEMENT_MODELDATA_MODELDATA_HPP_