#include "Configuration.hpp"

namespace Data {
    Configuration::Configuration(std::string configFile) {
        std::ifstream f(configFile.c_str());
        assert(f.good(), "Valid Config File Provided");

        read_ini(configFile, this->ptree);
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

    template <> double Configuration::get<double>(std::string str) {
        return this->ptree.get<double>(str);
    }

    template <> bool Configuration::get<bool>(std::string str) {
        return this->ptree.get<bool>(str);
    }

    template <> int Configuration::get<int>(std::string str) {
        return this->ptree.get<int>(str);
    }

    template <>
    std::vector<int> Configuration::get<std::vector<int>>(std::string str) {
        std::string res = this->ptree.get<std::string>(str);
        std::vector<int> resVec = this->parseString2VectorOfInts(res);
        std::vector<int> result;
        for (int r : resVec) {
            result.push_back(r);
        }
        return result;
    }

    template <>
    std::vector<std::string>
    Configuration::get<std::vector<std::string>>(std::string str) {
        std::string res = this->ptree.get<std::string>(str);
        if (res.empty()) {
            return {};
        }
        return this->parseString2VectorOfStrings(res);
    }
} // namespace Data
