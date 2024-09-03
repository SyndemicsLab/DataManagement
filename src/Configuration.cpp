#include "Configuration.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cassert>
#include <limits>
#include <type_traits>
#include <typeinfo>

namespace Data {
    class PTree {
    public:
        boost::property_tree::ptree ptree;
    };

    Config::Config() { dmTree = std::make_unique<PTree>(); }

    Config::Config(std::string configFile) {
        dmTree = std::make_unique<PTree>();
        std::ifstream f(configFile.c_str());
        assert((f.good(), "Valid Config File Provided"));

        read_ini(configFile, this->dmTree->ptree);
    }

    Config::~Config(){};

    ReturnType Config::get(std::string str, ReturnType default_value) {
        if (std::holds_alternative<int>(default_value)) {
            return this->dmTree->ptree.get<int>(str);
        } else if (std::holds_alternative<double>(default_value)) {
            return this->dmTree->ptree.get<double>(str);
        } else if (std::holds_alternative<float>(default_value)) {
            return this->dmTree->ptree.get<float>(str);
        } else if (std::holds_alternative<bool>(default_value)) {
            return this->dmTree->ptree.get<bool>(str);
        } else if (std::holds_alternative<char>(default_value)) {
            return this->dmTree->ptree.get<char>(str);
        } else {
            return this->dmTree->ptree.get<std::string>(str);
        }
    }

    std::vector<ReturnType> Config::getVector(std::string str) {

        try {
            std::string result =
                std::get<std::string>(this->get(str, std::string("")));

            return this->parse(result);
        } catch (const std::exception &e) {
            return {};
        }
    }

    std::shared_ptr<ReturnType> Config::get_optional(std::string str,
                                                     ReturnType default_value) {
        try {
            ReturnType val = this->get(str, default_value);
            // This is the string error check
            if (std::holds_alternative<std::string>(val) &&
                std::get<std::string>(val).empty()) {
                return nullptr;
            }
            return std::make_shared<ReturnType>(val);
        } catch (const boost::property_tree::ptree_error &e) {
            return nullptr;
        }
    }

    std::vector<ReturnType> Config::parse(std::string str,
                                          std::string delimiter) {
        std::vector<ReturnType> ret_vec;
        size_t pos = 0;
        std::string token;
        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            std::string trimmed_token = trim(token);
            ret_vec.push_back(this->convert_type(trimmed_token));
            str.erase(0, pos + delimiter.length());
            while (!str.empty() && str.at(0) == ' ') {
                str.erase(0, 1);
            }
        }
        if (!str.empty()) {
            ret_vec.push_back(this->convert_type(str));
        }

        return ret_vec;
    }

    std::vector<std::string> Config::getSectionCategories(std::string section) {
        boost::property_tree::ptree subTree =
            this->dmTree->ptree.get_child(section);
        std::vector<std::string> keyList;

        for (auto &key : subTree) {
            keyList.push_back(key.first);
        }
        return keyList;
    }

    ReturnType Config::convert_type(std::string str) {
        char *endptr;

        int i_res = std::strtol(str.c_str(), &endptr, 10);
        if (endptr != str.c_str() && *endptr == '\0') {
            return i_res;
        }

        double d_res = std::strtod(str.c_str(), &endptr);
        if (endptr != str.c_str() && *endptr == '\0') {
            return d_res;
        }

        float f_res = std::strtof(str.c_str(), &endptr);
        if (endptr != str.c_str() && *endptr == '\0') {
            return f_res;
        }
        return str;
    }

    std::string Config::trim(const std::string &str) {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first) {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
} // namespace Data
