#ifndef MODELDATA_HPP_
#define MODELDATA_HPP_

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <datamanagement/source/CSVSource.hpp>
#include <datamanagement/source/Config.hpp>
#include <datamanagement/source/DBSource.hpp>

namespace datamanagement {
    class ModelData {
    private:
        datamanagement::source::Config config;
        std::unordered_map<std::string, datamanagement::source::CSVSource>
            csvSources = {};
        std::unordered_map<std::string, datamanagement::source::DBSource>
            dbSources = {};

    public:
        ModelData(const std::string &cfgfile) : config(cfgfile) {}
        ~ModelData() = default;
        datamanagement::source::Config GetConfig() const { return config; }

        void AddSource(const std::string &path) {
            std::filesystem::path p = path;
            if (p.extension() == ".csv") {
                datamanagement::source::CSVSource s;
                csvSources[p.stem()] = std::move(s);
                csvSources[p.stem()].ConnectToFile(path);
            } else if (p.extension() == ".db") {
                datamanagement::source::DBSource s;
                dbSources[p.stem()] = std::move(s);
                dbSources[p.stem()].ConnectToDatabase(path);
            } else {
                // Not a valid source file
            }
        }

        std::vector<std::string> GetCSVSourceNames() const {
            std::vector<std::string> names = {};
            for (const auto &[k, v] : csvSources) {
                names.push_back(k);
            }
            return names;
        }
        std::vector<std::string> GetDBSourceNames() const {
            std::vector<std::string> names = {};
            for (const auto &[k, v] : dbSources) {
                names.push_back(k);
            }
            return names;
        }

        datamanagement::source::CSVSource &
        GetCSVSource(const std::string &name) {
            return csvSources[name];
        }

        datamanagement::source::DBSource &GetDBSource(const std::string &name) {
            return dbSources[name];
        }
    };
} // namespace datamanagement

#endif