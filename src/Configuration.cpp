#include "Configuration.hpp"

namespace Data {
    Configuration::Configuration(std::string configFile) {
        std::ifstream f(configFile.c_str());
        assert((f.good(), "Valid Config File Provided"));

        read_ini(configFile, this->ptree);
    }

    template <>
    std::shared_ptr<std::string>
    Configuration::optional<std::string>(std::string str) {
        boost::optional<std::string> result =
            this->ptree.get_optional<std::string>(str);
        std::cout << result.value() << std::endl;
        if (result.value() != "") {
            return std::make_shared<std::string>(*result);
        }
        return nullptr;
    }

    std::vector<std::string>
    Configuration::parseString2VectorOfStrings(std::string st) {
        std::stringstream ss(st);
        std::vector<std::string> result;

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
        std::vector<int> result;

        std::istringstream iss(st);
        std::string token;
        while (std::getline(iss, token, ',')) {
            result.push_back(std::stoi(token));
        }
        return result;
    }
} // namespace Data
