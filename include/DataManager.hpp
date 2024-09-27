#ifndef DATAMANAGER_HPP_
#define DATAMANAGER_HPP_

#include "DataManagerBase.hpp"
#include <memory>

namespace datamanagement {
    class DataManager : public DataManagerBase {
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
        DataManager(std::string const &dbfile);
        DataManager() = delete;
        ~DataManager();
        int AddCSVTable(std::string const &filepath) const override;
        int WriteTableToCSV(std::string const &filepath, std::string tablename,
                            std::string column_names) const override;
        int Create(std::string const query, Table &data) const override;
        int Select(std::string const query, Table &data) const override;
        int Update(std::string const query, Table &data) const override;
        int Delete(std::string const query, Table &data) const override;
        int SelectCustomCallback(std::string const query,
                                 int (*callback_func)(void *, int, char **,
                                                      char **),
                                 void *data, std::string &error) const override;
        int LoadConfig(std::string const &filepath) override;
        int GetFromConfig(std::string const key,
                          std::string &data) const override;
        int GetConfigSectionCategories(
            std::string const section,
            std::vector<std::string> &data) const override;
        int SaveDatabase(std::string const &outfile) override;
        int StartTransaction() const override;
        int EndTransaction() const override;

        // Copy Operations
        DataManager(DataManager const &) = delete;
        DataManager &operator=(DataManager const &) = delete;
        DataManager(DataManager &&) noexcept;
        DataManager &operator=(DataManager &&) noexcept;
    };
}; // namespace datamanagement

#endif