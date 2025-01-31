#ifndef DATASOURCE_HPP_
#define DATASOURCE_HPP_

#include <eigen3/Eigen/Dense>
#include <string>
#include <unordered_map>
#include <vector>

namespace datamanagement::source {
    class TableSource {
    protected:
        std::string name;

    public:
        virtual ~TableSource() = default;
        virtual Eigen::MatrixXd
        GetData(const std::vector<std::string> &select_columns,
                const std::unordered_map<std::string, std::string>
                    &where_conditions) const = 0;
        std::string GetName() const { return name; }
    };
} // namespace datamanagement::source

#endif