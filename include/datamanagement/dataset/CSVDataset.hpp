#ifndef CSVATASET_HPP_
#define CSVATASET_HPP_

#include <Dataset.hpp>
#include <string>
#include <vector>

namespace datamanagement {
    class CSVDataset : public virtual Dataset {
    public:
        virtual int AddCSV(std::string const &filepath) = 0;
    };
} // namespace datamanagement

#endif