#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace datamanagement::source {
    class Config {
    private:
        boost::property_tree::ptree ptree;
        std::string configfile = "";
        bool tree_loaded = false;

    public:
        Config() {}
        ~Config() = default;

        int LoadConfig(std::string const &filepath) {
            configfile = filepath;
            read_ini(filepath, this->ptree);
            tree_loaded = true;
            return 0;
        }

        std::string GetConfigFile() const { return configfile; }

        int GetFromConfig(std::string const key, std::string &data) const {
            if (!tree_loaded) {
                return -1;
            }
            try {
                data = ptree.get<std::string>(key);
            } catch (const std::exception &e) {
                // log bad cast
                return -1;
            }
            return 0;
        }

        int GetConfigSectionCategories(std::string const section,
                                       std::vector<std::string> &data) const {
            if (!tree_loaded) {
                return -1;
            }
            boost::property_tree::ptree subTree =
                this->ptree.get_child(section);
            std::vector<std::string> keyList;

            for (auto &key : subTree) {
                keyList.push_back(key.first);
            }
            data = keyList;
            return 0;
        }
    };
} // namespace datamanagement::source

#endif