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

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

/// @brief Namespace defining Data Containers
namespace Data {

    using ReturnType =
        std::variant<int, double, float, std::string, bool, char>;

    /// @brief Declaration of wrapper class for property tree parser
    class PTree;

    class IGettable {
    public:
        /// @brief Default template for getting parameters from the config
        /// @param str String key to search for
        /// @return The value searched for
        virtual ReturnType get(std::string, ReturnType) = 0;

        /// @brief Template for the optional parameters
        /// @param str String key to search for
        /// @return The optional value searched for
        virtual std::shared_ptr<ReturnType> get_optional(std::string,
                                                         ReturnType) = 0;
        virtual std::vector<ReturnType> getVector(std::string) = 0;

        [[deprecated("This function is not intended to be permanent. getVector "
                     "will recieve future focus.")]] virtual std::vector<int>
            getIntVector(std::string) = 0;
        [[deprecated("This function is not intended to be permanent. getVector "
                     "will recieve future focus.")]] virtual std::vector<double>
            getDoubleVector(std::string) = 0;
        [[deprecated(
            "This function is not intended to be permanent. getVector will "
            "recieve future focus.")]] virtual std::vector<std::string>
            getStringVector(std::string) = 0;
    };

    class IParseable {
    public:
        /// @brief Parse a single string to a vector
        /// @param  string to parse
        /// @return vector containing parsed string
        virtual std::vector<ReturnType> parse(std::string) = 0;

        /// @brief Parse a single string to a vector
        /// @param  string to parse
        /// @param delimiter string is being parsed with
        /// @return vector containing parsed string
        virtual std::vector<ReturnType> parse(std::string, std::string) = 0;
    };

    /// @brief Interface used for Configuration, provides guaranteed methods
    class IConfigable : public virtual IGettable, public virtual IParseable {
    public:
        virtual std::vector<std::string>
        getSectionCategories(std::string section) = 0;
    };

    /// @brief Type definition for an easy to use pointer to a configuration
    /// interface
    using IConfigablePtr = std::shared_ptr<Data::IConfigable>;

    /// @brief Class describing a standard configuration file
    class Config : public IConfigable {
    private:
        std::unique_ptr<PTree> dmTree;
        ReturnType convert_type(std::string);
        std::string trim(const std::string &str);

    public:
        /// @brief Default constructor with no file
        Config();

        /// @brief Primary Constructor used with a file
        /// @param configFile string path to the config file
        Config(std::string configFile);

        /// @brief Default Destructor
        ~Config();

        ReturnType get(std::string str, ReturnType default_value) override;

        std::vector<ReturnType> getVector(std::string str) override;

        std::shared_ptr<ReturnType>
        get_optional(std::string str, ReturnType default_value) override;

        std::vector<ReturnType> parse(std::string str) override {
            return this->parse(str, ",");
        }

        /// @brief Helper function to parse a string to a vector of strings
        /// @param st string to parse
        /// @return vector of parsed strings
        std::vector<ReturnType> parse(std::string str,
                                      std::string delimiter) override;

        /// @brief Helper function to return all the categories belonging to the
        /// provided section
        /// @param section String describing the section to search
        /// @return A vector of the categories avaliable
        std::vector<std::string>
        getSectionCategories(std::string section) override;

        std::vector<int> getIntVector(std::string str) override {
            std::string numbers = "";
            numbers = std::get<std::string>(this->get(str, numbers));
            std::vector<ReturnType> returned = this->parse(numbers);
            std::vector<int> casted_returned = {};
            std::for_each(returned.begin(), returned.end(), [&](ReturnType &n) {
                casted_returned.push_back(std::get<int>(n));
            });
            return casted_returned;
        }

        std::vector<double> getDoubleVector(std::string str) override {
            std::string numbers = "";
            numbers = std::get<std::string>(this->get(str, numbers));
            std::vector<ReturnType> returned = this->parse(numbers);
            std::vector<double> casted_returned = {};
            std::for_each(returned.begin(), returned.end(), [&](ReturnType &n) {
                casted_returned.push_back(std::get<double>(n));
            });
            return casted_returned;
        }

        std::vector<std::string> getStringVector(std::string str) override {
            std::string strings = "";
            strings = std::get<std::string>(this->get(str, strings));
            std::vector<ReturnType> returned = this->parse(strings);
            std::vector<std::string> casted_returned;
            std::for_each(
                returned.begin(), returned.end(), [&](Data::ReturnType &n) {
                    casted_returned.push_back(std::get<std::string>(n));
                });
            return casted_returned;
        }
    };
} // namespace Data
#endif
