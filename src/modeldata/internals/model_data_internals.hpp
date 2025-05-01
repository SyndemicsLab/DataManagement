////////////////////////////////////////////////////////////////////////////////
// File: model_data_internals.hpp                                             //
// Project: internals                                                         //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-01                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef DATAMANAGEMENT_SRC_MODELDATA_MODELDATAINTERNALS_HPP_
#define DATAMANAGEMENT_SRC_MODELDATA_MODELDATAINTERNALS_HPP_

#include <datamanagement/modeldata/model_data.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace datamanagement {
class ModelDataImpl : public virtual ModelData {
public:
    ModelDataImpl(const std::string &cfgfile,
                  const std::string &log_name = "console");
    ~ModelDataImpl() = default;

    std::string GetFromConfig(const std::string &key) const override;

    std::vector<std::string>
    GetConfigSectionCategories(const std::string &section) const override;

    void AddSource(const std::string &path) override;

    std::vector<std::string> GetCSVSourceNames() const override;
    std::vector<std::string> GetDBSourceNames() const override;

    inline datamanagement::source::CSVSource &
    GetCSVSource(const std::string &name) override {
        return _csv_sources[name];
    }

    inline datamanagement::source::DBSource &
    GetDBSource(const std::string &name) override {
        return _db_sources.at(name);
    }

private:
    const std::string _log_name;
    boost::property_tree::ptree _ptree;
    std::unordered_map<std::string, source::CSVSource> _csv_sources = {};
    std::unordered_map<std::string, source::DBSource> _db_sources = {};
};
} // namespace datamanagement

#endif // DATAMANAGEMENT_LIB_MODELDATA_MODELDATAINTERNALS_HPP_