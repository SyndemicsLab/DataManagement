////////////////////////////////////////////////////////////////////////////////
// File: model_data.hpp                                                       //
// Project: modeldata                                                         //
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

#ifndef DATAMANAGEMENT_MODELDATA_MODELDATA_HPP_
#define DATAMANAGEMENT_MODELDATA_MODELDATA_HPP_

#include <memory>
#include <string>
#include <vector>

#include <datamanagement/source/csv_source.hpp>
#include <datamanagement/source/db_source.hpp>

namespace datamanagement {
class ModelData {
public:
    virtual ~ModelData() = default;
    virtual std::string GetFromConfig(const std::string &key) const = 0;

    virtual std::vector<std::string>
    GetConfigSectionCategories(const std::string &section) const = 0;

    virtual void AddSource(const std::string &path) = 0;

    virtual std::vector<std::string> GetCSVSourceNames() const = 0;
    virtual std::vector<std::string> GetDBSourceNames() const = 0;

    virtual source::CSVSource &GetCSVSource(const std::string &name) = 0;

    virtual source::DBSource &GetDBSource(const std::string &name) = 0;

    static std::unique_ptr<ModelData>
    Create(const std::string &config, const std::string &log_name = "console");
};
} // namespace datamanagement

#endif // DATAMANAGEMENT_MODELDATA_MODELDATA_HPP_