////////////////////////////////////////////////////////////////////////////////
// File: csv_source.hpp                                                       //
// Project: source                                                            //
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

#ifndef CSVDATASOURCE_HPP_
#define CSVDATASOURCE_HPP_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <datamanagement/utils/csv.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace datamanagement::source {
class CSVSource {
private:
    std::string filepath;

    bool CheckWhereConditions(const csv::CSVRow &row,
                              const std::unordered_map<std::string, std::string>
                                  &where_conditions) const {
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

public:
    CSVSource() {}
    ~CSVSource() = default;

    void ConnectToFile(const std::string &s) { filepath = s; }

    std::string GetName() const {
        std::filesystem::path p = filepath;
        return p.filename().string();
    }

    Eigen::MatrixXd GetData(const std::vector<std::string> &select_columns,
                            const std::unordered_map<std::string, std::string>
                                &where_conditions) const {
        std::vector<std::vector<double>> temp_data;
        csv::CSVReader reader(filepath);
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

    virtual void WriteCSV(std::string const &filepath,
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
};
} // namespace datamanagement::source

#endif