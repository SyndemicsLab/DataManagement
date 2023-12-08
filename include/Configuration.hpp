#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include "DataTable.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cassert>

namespace Data {
    class Configuration {
    private:
        boost::property_tree::ptree ptree;

    public:
        // CONSTRUCTORS
        Configuration(){};
        Configuration(std::string configFile);
        virtual ~Configuration() = default;

        /// @brief
        /// @param T
        /// @param str
        /// @return
        template <typename T> T get(std::string str) {
            return this->ptree.get<T>(str);
        }

        /// @brief
        /// @param T
        /// @param str
        /// @return
        template <typename T> T optional(std::string str) {
            boost::optional<T> result = this->ptree.get_optional<T>(str);
            if (result) {
                return *result;
            }
            return T();
        }

        /// @brief
        /// @param
        /// @return
        std::vector<std::string> parseString2VectorOfStrings(std::string st);
        /// @brief
        /// @param
        /// @return
        std::vector<int> parseString2VectorOfInts(std::string st);
    };
} // namespace Data

#endif
