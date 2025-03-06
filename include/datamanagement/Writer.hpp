////////////////////////////////////////////////////////////////////////////////
// File: Writer.hpp                                                           //
// Project: DataManagement                                                    //
// Created Date: 2025-03-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef WRITER_HPP_
#define WRITER_HPP_

#include <datamanagement/DataTable.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace Data {
    class Writer {
    public:
        static void writeCSV(const std::string &filename, DataTable table) {
            table.toCSV(filename);
        }
    };
} // namespace Data

#endif