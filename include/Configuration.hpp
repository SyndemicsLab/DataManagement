#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <memory>

/// @brief Namespace defining Data Containers
namespace Data {

    /// @brief Class describing a standard configuration file
    class Configuration {
    private:
        boost::property_tree::ptree ptree;

    public:
        // CONSTRUCTORS

        /// @brief Default constructor with no file
        Configuration(){};

        /// @brief Primary Constructor used with a file
        /// @param configFile string path to the config file
        Configuration(std::string configFile);

        /// @brief Default destructor
        virtual ~Configuration() = default;

        /// @brief Default template for getting parameters from the config
        /// @param T Type to return
        /// @param str String key to search for
        /// @return The value searched for of type T
        template <typename T> T get(std::string str) {
            return this->ptree.get<T>(str);
        }

        /// @brief Template for the optional parameters
        /// @param T Type to return
        /// @param str String key to search for
        /// @return The optional value searched for of type T
        template <typename T> std::shared_ptr<T> optional(std::string str) {
            boost::optional<T> result = this->ptree.get_optional<T>(str);
            if (result) {
                return std::make_shared<T>(*result);
            }
            return nullptr;
        }

        /// @brief An overload of \code{optional} for strings
        /// @param str String key to search for
        /// @return The optional value searched for of type string
        std::shared_ptr<std::string> optional(std::string str);

        /// @brief Helper function to parse a string to a vector of strings
        /// @param st string to parse
        /// @return vector of parsed strings
        std::vector<std::string> parseString2VectorOfStrings(std::string st);

        /// @brief Helper function to parse a string to a vector of ints
        /// @param st string to parse
        /// @return vector of parsed ints
        std::vector<int> parseString2VectorOfInts(std::string st);
    };
} // namespace Data
#endif
