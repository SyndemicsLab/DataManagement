//===-- Configuration.hpp ----------------------------------------*- C++-*-===//
//
// Part of the Syndemics Lab DataManagement Code Repository, under
// the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of the Configuration interface and
/// base class. In addition to the class definitions, a typedef is provded
/// to utilize a pointer of the Configuration object.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <memory>
#include <string>
#include <variant>
#include <vector>

/// @brief Namespace defining Data Containers
namespace Data {

    /// @brief Declaration of wrapper class for property tree parser
    class PTree;

    /// @brief Interface used for Configuration, provides guaranteed methods
    class IConfiguration {
    public:
        virtual ~IConfiguration() = default;
        virtual std::vector<std::string>
        parseString2VectorOfStrings(std::string st) = 0;
        virtual std::vector<int> parseString2VectorOfInts(std::string st) = 0;
        virtual std::vector<std::string> getStringVector(std::string str) = 0;
        virtual std::vector<int> getIntVector(std::string str) = 0;
        virtual std::vector<std::string>
        getSectionCategories(std::string section) = 0;
    };

    /// @brief Type definition for an easy to use pointer to a configuration
    /// interface
    using IConfigurationPtr = std::shared_ptr<Data::IConfiguration>;

    /// @brief Class describing a standard configuration file
    class Configuration : public IConfiguration {
    private:
        std::unique_ptr<PTree> dmTree;

    public:
        /// @brief Default constructor with no file
        Configuration();

        /// @brief Primary Constructor used with a file
        /// @param configFile string path to the config file
        Configuration(std::string configFile);

        /// @brief Default Destructor
        ~Configuration();

        /// @brief Default template for getting parameters from the config
        /// @param T Type to return
        /// @param str String key to search for
        /// @return The value searched for of type T
        template <typename T> T get(std::string str);

        /// @brief Template for the optional parameters
        /// @param T Type to return
        /// @param str String key to search for
        /// @return The optional value searched for of type T
        template <typename T> std::shared_ptr<T> optional(std::string str);

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

        /// @brief Helper function to return all the categories belonging to the
        /// provided section
        /// @param section String describing the section to search
        /// @return A vector of the categories avaliable
        std::vector<std::string>
        getSectionCategories(std::string section) override;
    };
} // namespace Data
#endif
