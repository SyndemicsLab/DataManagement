#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include "DataTable.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Data {
    class Configuration {
    private:
        boost::property_tree::ptree ptree;

        std::vector<std::string> parseString2VectorOfStrings(std::string st);
        std::vector<int> parseString2VectorOfInts(std::string st);
        std::vector<int> updateIndices(std::vector<int> indices,
                                       std::vector<int> maxIndices);

    public:
        // CONSTRUCTORS
        Configuration(){};
        Configuration(std::string configFile);
        virtual ~Configuration() = default;

        /// @brief
        /// @tparam T
        /// @param str
        /// @return
        template <typename T> T get(std::string str);
    };
} // namespace Data

#endif