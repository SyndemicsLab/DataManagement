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
        std::vector<datamanagement::source::CSVSource> csvSources = {};
        std::vector<datamanagement::source::DBSource> dbSources = {};

    public:
        ModelData(const std::string &cfgfile) : config(cfgfile) {}
        ~ModelData() = default;
        datamanagement::source::Config GetConfig() const { return config; }

        void AddSource(const std::string &path) {
            std::filesystem::path p = path;
            if (p.extension() == ".csv") {
                csvSources.emplace_back(path);
            } else if (p.extension() == ".db") {
                dbSources.emplace_back(path);
            } else {
                // Not a valid source file
            }
        }

        std::vector<std::string> GetCSVSourceNames() const {
            std::vector<std::string> names = {};
            for (const datamanagement::source::CSVSource &src : csvSources) {
                names.push_back(src.GetName());
            }
            return names;
        }
        std::vector<std::string> GetDBSourceNames() const {
            std::vector<std::string> names = {};
            for (const datamanagement::source::DBSource &src : dbSources) {
                names.push_back(src.GetName());
            }
            return names;
        }
    };
} // namespace datamanagement

#endif