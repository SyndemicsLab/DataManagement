#ifndef MODELDATA_HPP_
#define MODELDATA_HPP_

#include <memory>
#include <string>
#include <vector>

namespace datamanagement {
    class Dataset;
    class ModelData {
    public:
        virtual ~ModelData() = default;
        virtual void AddConfig(const std::string &path) = 0;
        virtual void AddDataset(const Dataset &dataset) = 0;
        virtual std::vector<Dataset> GetDatasets() const = 0;

        virtual std::string GetConfigFile() const = 0;
        virtual int GetFromConfig(std::string const key,
                                  std::string &data) const = 0;
        virtual int
        GetConfigSectionCategories(std::string const section,
                                   std::vector<std::string> &data) const = 0;

        static std::unique_ptr<ModelData> Create();
    };
} // namespace datamanagement

#endif