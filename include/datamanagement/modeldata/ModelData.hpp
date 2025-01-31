#ifndef MODELDATA_HPP_
#define MODELDATA_HPP_

#include <memory>
#include <string>
#include <vector>

namespace datamanagement {
    class Source;
    class ModelData {
    public:
        ModelData() {}
        virtual ~ModelData() = default;

        void AddConfig(const std::string &path) {}
        void AddSource(const std::string &path) {}
        void AddSource(const Source &datasource) {}
        std::vector<Source> GetSources() const {}

        std::string GetConfigFile() const {}
        int GetFromConfig(std::string const key, std::string &data) const {}
        int GetConfigSectionCategories(std::string const section,
                                       std::vector<std::string> &data) const {}
    };
} // namespace datamanagement

#endif