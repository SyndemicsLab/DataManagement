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
    class DataManager {
    protected:
        std::string quoter(const std::string &s) const {
            return std::string("'") + s + std::string("'");
        }

    private:
        class Database;
        class Config;
        std::unique_ptr<Database> pImplDB;
        std::unique_ptr<Config> pImplCF;

    public:
        DataManager();
        ~DataManager();
        int AddCSVTable(std::string const &filepath) const;
        int Create(std::string const query, Table &data) const;
        int Select(std::string const query, Table &data) const;
        int Update(std::string const query, Table &data) const;
        int Delete(std::string const query, Table &data) const;
        int SelectCustomCallback(std::string const query,
                                 int (*callback_func)(void *, int, char **,
                                                      char **),
                                 void *data, std::string &error) const;
        int LoadConfig(std::string const &filepath);
        int GetFromConfig(std::string const key, std::string &data) const;
        int GetConfigSectionCategories(std::string const section,
                                       std::vector<std::string> &data) const;
    };
}; // namespace datamanagement

#endif