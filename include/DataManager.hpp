#ifndef DATAMANAGER_HPP_
#define DATAMANAGER_HPP_

#include <memory>
#include <string>
#include <vector>

namespace datamanagement {
    class DataManagerBase {
    private:
        class DataManager;
        std::unique_ptr<DataManager> pImpl;

    public:
        DataManagerBase();
        ~DataManagerBase();
        int AddCSVTable(std::string const &) const;
        int Create(std::string const, std::vector<std::string> data) const;
        int Select(std::string const, std::vector<std::string> data) const;
        int Update(std::string const, std::vector<std::string> data) const;
        int Delete(std::string const, std::vector<std::string> data) const;
    };
}; // namespace datamanagement

#endif