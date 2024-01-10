#include "Configuration.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cassert>

namespace Data {
    class PTree {
    public:
        boost::property_tree::ptree ptree;
    };

    /// @brief Default template for getting parameters from the config
    /// @param T Type to return
    /// @param str String key to search for
    /// @return The value searched for of type T
    template <typename T> T Configuration::get(std::string str) {
        return this->dmTree->ptree.get<T>(str);
    }

    template int Configuration::get<int>(std::string str);
    template bool Configuration::get<bool>(std::string str);
    template float Configuration::get<float>(std::string str);
    template double Configuration::get<double>(std::string str);
    template char Configuration::get<char>(std::string str);
    template std::string Configuration::get<std::string>(std::string str);

    void Configuration::get(
        std::string str,
        std::variant<int, bool, float, double, char, std::string> &value) {
        if (std::holds_alternative<int>(value)) {
            value = this->get<int>(str);
        } else if (std::holds_alternative<bool>(value)) {
            value = this->get<bool>(str);
        } else if (std::holds_alternative<float>(value)) {
            value = this->get<float>(str);
        } else if (std::holds_alternative<double>(value)) {
            value = this->get<double>(str);
        } else if (std::holds_alternative<char>(value)) {
            value = this->get<char>(str);
        } else if (std::holds_alternative<std::string>(value)) {
            value = this->get<std::string>(str);
        }
    }

    /// @brief Template for the optional parameters
    /// @param T Type to return
    /// @param str String key to search for
    /// @return The optional value searched for of type T
    template <typename T>
    std::shared_ptr<T> Configuration::optional(std::string str) {
        boost::optional<T> result = this->dmTree->ptree.get_optional<T>(str);
        if (result) {
            return std::make_shared<T>(*result);
        }
        return nullptr;
    }

    template std::shared_ptr<int> Configuration::optional<int>(std::string str);
    template std::shared_ptr<bool>
    Configuration::optional<bool>(std::string str);
    template std::shared_ptr<float>
    Configuration::optional<float>(std::string str);
    template std::shared_ptr<double>
    Configuration::optional<double>(std::string str);
    template std::shared_ptr<char>
    Configuration::optional<char>(std::string str);
    template std::shared_ptr<std::string>
    Configuration::optional<std::string>(std::string str);

    void Configuration::optional(
        std::string str,
        std::variant<std::shared_ptr<int>, std::shared_ptr<bool>,
                     std::shared_ptr<float>, std::shared_ptr<double>,
                     std::shared_ptr<char>, std::shared_ptr<std::string>>
            &value) {

        if (std::holds_alternative<std::shared_ptr<int>>(value)) {
            value = this->optional<int>(str);
        } else if (std::holds_alternative<std::shared_ptr<bool>>(value)) {
            value = this->optional<bool>(str);
        } else if (std::holds_alternative<std::shared_ptr<float>>(value)) {
            value = this->optional<float>(str);
        } else if (std::holds_alternative<std::shared_ptr<double>>(value)) {
            value = this->optional<double>(str);
        } else if (std::holds_alternative<std::shared_ptr<char>>(value)) {
            value = this->optional<char>(str);
        } else if (std::holds_alternative<std::shared_ptr<std::string>>(
                       value)) {
            value = this->optional<std::string>(str);
        }
    }

    Configuration::Configuration(std::string configFile) {
        std::ifstream f(configFile.c_str());
        assert((f.good(), "Valid Config File Provided"));

        read_ini(configFile, this->dmTree->ptree);
    }

    std::vector<std::string>
    Configuration::parseString2VectorOfStrings(std::string st) {
        std::stringstream ss(st);
        std::vector<std::string> result = {};

        while (ss.good()) {
            std::string substr;
            getline(ss, substr, ',');
            // remove leading whitespace
            int first = substr.find_first_not_of(' ');
            // break and return if the string is empty
            // TODO log warning when this happens
            if (std::string::npos == first) {
                break;
            }
            int last = substr.find_last_not_of(' ');
            result.push_back(substr.substr(first, (last - first + 1)));
        }
        return result;
    }

    std::vector<int> Configuration::parseString2VectorOfInts(std::string st) {
        std::vector<int> result = {};

        std::istringstream iss(st);
        std::string token;
        while (std::getline(iss, token, ',')) {
            result.push_back(std::stoi(token));
        }
        return result;
    }
} // namespace Data
