////////////////////////////////////////////////////////////////////////////////
// File: model_data.cpp                                                       //
// Project: modeldata                                                         //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-01                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <datamanagement/modeldata/model_data.hpp>

#include <datamanagement/utils/logging.hpp>

#include "internals/model_data_internals.hpp"

namespace datamanagement {
ModelDataImpl::ModelDataImpl(const std::string &config,
                             const std::string &log_name)
    : _log_name(log_name) {
    read_ini(config, _ptree);
}

void ModelDataImpl::AddSource(const std::string &path) {
    std::filesystem::path p = path;
    if (p.extension() == ".csv") {
        datamanagement::source::CSVSource s;
        _csv_sources[p.stem()] = std::move(s);
        _csv_sources[p.stem()].ConnectToFile(path);
    } else if (p.extension() == ".db") {
        datamanagement::source::DBSource s;
        _db_sources[p.stem()] = std::move(s);
        _db_sources[p.stem()].SetDatabasePath(path);
    } else {
        datamanagement::utils::LogWarning(
            _log_name, path + " is an invalid invalid source file...");
    }
}

std::vector<std::string> ModelDataImpl::GetCSVSourceNames() const {
    std::vector<std::string> names = {};
    for (const auto &[k, v] : _csv_sources) {
        names.push_back(k);
    }
    return names;
}

std::vector<std::string> ModelDataImpl::GetDBSourceNames() const {
    std::vector<std::string> names = {};
    for (const auto &[k, v] : _db_sources) {
        names.push_back(k);
    }
    return names;
}

// Public Methods
std::string ModelDataImpl::GetFromConfig(const std::string &key) const {
    std::string result;
    try {
        result = _ptree.get<std::string>(key);
    } catch (const std::exception &e) {
        // log bad cast
        datamanagement::utils::LogError(_log_name,
                                        "Error in attempting to extract " +
                                            key + " from config file...");
    }
    return result;
}

std::vector<std::string>
ModelDataImpl::GetConfigSectionCategories(const std::string &section) const {
    boost::property_tree::ptree sub_tree = _ptree.get_child(section);
    std::vector<std::string> key_list;

    for (auto &key : sub_tree) {
        key_list.push_back(key.first);
    }
    return key_list;
}

std::unique_ptr<ModelData> ModelData::Create(const std::string &config,
                                             const std::string &log_name) {
    return std::make_unique<ModelDataImpl>(config, log_name);
}
} // namespace datamanagement