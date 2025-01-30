#ifndef CSVATASET_HPP_
#define CSVATASET_HPP_

#include <datamanagement/datasource/DataSource.hpp>
#include <string>
#include <vector>

namespace datamanagement {
    class CSVDataSource : public virtual DataSource {
    public:
        void AddData(const std::string &s) const override {}
        Eigen::MatrixXd GetData(const std::string &query) const override {}

        virtual void ReadCSV(std::string const &filepath) { AddData(filepath); }

        virtual void WriteCSV(std::string const &filepath) const {
            std::ofstream file(filepath.c_str());
            file << GetData("").format(CSVFormat);
        }
    };
} // namespace datamanagement

#endif