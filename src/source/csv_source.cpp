////////////////////////////////////////////////////////////////////////////////
// File: csv_source.cpp                                                       //
// Project: source                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-01                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <datamanagement/source/csv_source.hpp>

namespace datamanagement {
namespace source {
bool CSVSource::CheckWhereConditions(
    const csv::CSVRow &row,
    const std::unordered_map<std::string, std::string> &where_conditions)
    const {
    if (where_conditions.empty()) {
        return true;
    }
    for (auto &condition : where_conditions) {
        if (row[condition.first].get<std::string>() != condition.second) {
            return false;
        }
    }
    return true;
}

Eigen::MatrixXd
CSVSource::GetData(const std::vector<std::string> &select_columns,
                   const std::unordered_map<std::string, std::string>
                       &where_conditions) const {
    std::vector<std::vector<double>> temp_data;
    csv::CSVReader reader(_filepath);
    for (csv::CSVRow &row : reader) {
        if (CheckWhereConditions(row, where_conditions)) {
            std::vector<double> temp_row;
            for (const std::string &col : select_columns) {
                temp_row.push_back(row[col].get<double>());
            }
            temp_data.push_back(temp_row);
        }
    }
    Eigen::MatrixXd data(temp_data.size(), select_columns.size());
    for (int i = 0; i < temp_data.size(); ++i) {
        data.row(i) = Eigen::Map<Eigen::VectorXd>(temp_data[i].data(),
                                                  temp_data[i].size());
    }
    return data;
}

void CSVSource::WriteCSV(std::string const &filepath,
                         const std::vector<std::string> &columns) const {
    std::ofstream file(filepath.c_str());
    for (const std::string &col : columns) {
        file << col;
        if (&col != &columns.back()) {
            file << ",";
        } else {
            file << "\n";
        }
    }
    file << GetData(columns, {})
                .format(Eigen::IOFormat(Eigen::StreamPrecision,
                                        Eigen::DontAlignCols, ",", "\n"));
    file.close();
}
} // namespace source
} // namespace datamanagement