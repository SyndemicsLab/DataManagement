#ifndef READER_HPP_
#define READER_HPP_

#include "DataTable.hpp"
#include <string>
#include <vector>

namespace Data {
    class Reader {
    private:
    public:
        DataTable read_csv(std::string);
        std::vector<DataTable> read_csvs_from_dir(std::string);
    };
} // namespace Data

#endif