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

    public:
        Config(const std::string &path) { read_ini(path, ptree); }
        ~Config() = default;

        void GetFromConfig(std::string const key, std::string &data) const {
            try {
                data = ptree.get<std::string>(key);
            } catch (const std::exception &e) {
                // log bad cast
            }
        }

        void GetConfigSectionCategories(std::string const section,
                                        std::vector<std::string> &data) const {
            boost::property_tree::ptree subTree =
                this->ptree.get_child(section);
            std::vector<std::string> keyList;

            for (auto &key : subTree) {
                keyList.push_back(key.first);
            }
            data = keyList;
        }
    };
} // namespace datamanagement::source

#endif