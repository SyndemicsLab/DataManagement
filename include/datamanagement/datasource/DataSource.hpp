#ifndef DATASOURCE_HPP_
#define DATASOURCE_HPP_

#include <eigen3/Eigen/Dense>
#include <string>
#include <vector>

namespace datamanagement {
    class DataSource {
    protected:
        std::string name;

    public:
        virtual void AddData(const std::string &s) const = 0;
        virtual Eigen::MatrixXd GetData(const std::string &query) const = 0;
    };
} // namespace datamanagement

#endif