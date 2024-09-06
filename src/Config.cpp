#include "DataManager.hpp"
#include <boost/property_tree/ptree.hpp>

namespace datamanagement {
    class DataManagerBase::Config {
    private:
        boost::property_tree::ptree ptree;

    public:
        Config::Config() {}
        Config::~Config() = default;
        int GetFromConfig(std::string const key, std::any &data) const {
            try {
                data = ptree.get(key, data);
            } catch (const std::bad_any_cast &e) {
                // log bad cast
                return -1;
            }
            return 0;
        }

        std::vector<std::string>
        GetConfigSectionCategories(std::string const section) const {
            boost::property_tree::ptree subTree =
                this->ptree.get_child(section);
            std::vector<std::string> keyList;

            for (auto &key : subTree) {
                keyList.push_back(key.first);
            }
            return keyList;
        }
    };

    int DataManagerBase::GetFromConfig(std::string const key,
                                       std::any &data) const {
        return pImplCF->GetFromConfig(key, data);
    }

    std::vector<std::string> DataManagerBase::GetConfigSectionCategories(
        std::string const section) const {
        return pImplCF->GetConfigSectionCategories(section);
    }
} // namespace datamanagement