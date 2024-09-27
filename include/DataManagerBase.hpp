#ifndef DATAMANAGERBASE_HPP_
#define DATAMANAGERBASE_HPP_

#include <string>
#include <vector>

namespace datamanagement {
    using Row = std::vector<std::string>;
    using Table = std::vector<Row>;
    class DataManagerBase {
    public:
        virtual int AddCSVTable(std::string const &filepath) const = 0;
        virtual int WriteTableToCSV(std::string const &filepath,
                                    std::string tablename,
                                    std::string column_names) const = 0;
        virtual int Create(std::string const query, Table &data) const = 0;
        virtual int Select(std::string const query, Table &data) const = 0;
        virtual int Update(std::string const query, Table &data) const = 0;
        virtual int Delete(std::string const query, Table &data) const = 0;
        virtual int SelectCustomCallback(std::string const query,
                                         int (*callback_func)(void *, int,
                                                              char **, char **),
                                         void *data,
                                         std::string &error) const = 0;
        virtual int LoadConfig(std::string const &filepath) = 0;
        virtual int GetFromConfig(std::string const key,
                                  std::string &data) const = 0;
        virtual int
        GetConfigSectionCategories(std::string const section,
                                   std::vector<std::string> &data) const = 0;
        virtual int SaveDatabase(std::string const &outfile) = 0;
        virtual int StartTransaction() const = 0;
        virtual int EndTransaction() const = 0;
        virtual std::string GetDBFileName() const = 0;
    };
} // namespace datamanagement

#endif