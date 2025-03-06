////////////////////////////////////////////////////////////////////////////////
// File: Reader.hpp                                                           //
// Project: DataManagement                                                    //
// Created Date: 2025-03-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef READER_HPP_
#define READER_HPP_

#include <datamanagement/DataTable.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace Data {
    class Reader {
    public:
        static DataTable readCSV(std::string &filename,
                                 bool hasHeaders = true) {
            DataTable dt;
            dt.fromCSV(filename, hasHeaders);
            return dt;
        }

        static std::vector<DataTable> read_csvs_from_dir(std::string &dirname) {
            std::vector<DataTable> dataTables;
            for (const auto &dirEntry :
                 std::filesystem::recursive_directory_iterator(dirname)) {
                if (!dirEntry.is_directory()) {
                    DataTable dt;
                    if (dt.fromCSV(dirEntry.path().string())) {
                        dataTables.push_back(dt);
                    }
                }
            }
            return dataTables;
        }
    };
} // namespace Data

#endif