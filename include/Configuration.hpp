#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <memory>

/// @brief Namespace defining Data Containers
namespace Data {

    class IConfiguration {
    public:
        virtual ~IConfiguration() = default;
        virtual std::string get(std::string str) = 0;
        virtual std::shared_ptr<std::string> optional(std::string str) = 0;
        virtual std::vector<std::string>
        parseString2VectorOfStrings(std::string st) = 0;
        virtual std::vector<int> parseString2VectorOfInts(std::string st) = 0;
        virtual std::vector<std::string> getStringVector(std::string str) = 0;
        virtual std::vector<int> getIntVector(std::string str) = 0;
    };

    /// @brief Class describing a standard configuration file
    class Configuration : public IConfiguration {
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

        std::string get(std::string str) override {
            return this->ptree.get<std::string>(str);
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
        std::shared_ptr<std::string> optional(std::string str) override;

        /// @brief Helper function to parse a string to a vector of strings
        /// @param st string to parse
        /// @return vector of parsed strings
        std::vector<std::string>
        parseString2VectorOfStrings(std::string st) override;

        /// @brief Helper function to parse a string to a vector of ints
        /// @param st string to parse
        /// @return vector of parsed ints
        std::vector<int> parseString2VectorOfInts(std::string st) override;

        /// @brief Wrapper function around `optional` and
        /// `parseString2VectorofStrings`.
        /// @param str string to parse
        /// @return vector of parsed strings
        std::vector<std::string> getStringVector(std::string str) override {
            std::shared_ptr<std::string> value =
                this->optional<std::string>(str);
            if (value) {
                return this->parseString2VectorOfStrings(*value);
            } else {
                return {};
            }
        }

        /// @brief Wrapper function around `optional` and
        /// `parseString2VectorofInts`.
        /// @param str string to parse
        /// @return vector of parsed ints
        std::vector<int> getIntVector(std::string str) override {
            std::shared_ptr<std::string> value =
                this->optional<std::string>(str);
            if (value) {
                return this->parseString2VectorOfInts(*value);
            } else {
                return {};
            }
        }
    };
} // namespace Data
#endif
