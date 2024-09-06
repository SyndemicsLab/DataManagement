#ifndef DATAMANAGER_HPP_
#define DATAMANAGER_HPP_

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace datamanagement {
    using Row = std::vector<std::string>;
    using Table = std::vector<Row>;
    class DataManagerBase {
    private:
        class DataManager;
        class Config;
        std::unique_ptr<DataManager> pImplDM;
        std::unique_ptr<Config> pImplCF;

    public:
        DataManagerBase();
        ~DataManagerBase();
        int AddCSVTable(std::string const &filepath) const;
        int Create(std::string const query, Table data) const;
        int Select(std::string const query, Table data) const;
        int Update(std::string const query, Table data) const;
        int Delete(std::string const query, Table data) const;
        int GetFromConfig(std::string const key, std::any &data) const;
        std::vector<std::string>
        GetConfigSectionCategories(std::string const section) const;
    };
}; // namespace datamanagement

#endif