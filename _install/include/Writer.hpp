#ifndef WRITER_HPP_
#define WRITER_HPP_

#include "DataTable.hpp"
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