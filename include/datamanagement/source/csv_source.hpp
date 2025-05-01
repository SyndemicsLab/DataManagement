////////////////////////////////////////////////////////////////////////////////
// File: csv_source.hpp                                                       //
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

#ifndef DATAMANAGEMENT_SOURCE_CSVSOURCE_HPP_
#define DATAMANAGEMENT_SOURCE_CSVSOURCE_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <datamanagement/utils/csv.hpp>

namespace datamanagement::source {
class CSVSource {
public:
    CSVSource() = default;
    ~CSVSource() = default;

    inline void ConnectToFile(const std::string &s) { _filepath = s; }

    inline std::string GetName() const {
        std::filesystem::path p = _filepath;
        return p.filename().string();
    }

    Eigen::MatrixXd GetData(const std::vector<std::string> &select_columns,
                            const std::unordered_map<std::string, std::string>
                                &where_conditions) const;

    void WriteCSV(std::string const &filepath,
                  const std::vector<std::string> &columns) const;

private:
    std::string _filepath;

    bool CheckWhereConditions(const csv::CSVRow &row,
                              const std::unordered_map<std::string, std::string>
                                  &where_conditions) const;
};
} // namespace datamanagement::source

#endif // DATAMANAGEMENT_SOURCE_CSVSOURCE_HPP_