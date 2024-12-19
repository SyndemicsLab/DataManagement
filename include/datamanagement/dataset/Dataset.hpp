#ifndef DATASET_HPP_
#define DATASET_HPP_

#include <string>
#include <vector>

namespace datamanagement {
    class Dataset {
    public:
        virtual int GetRun() const = 0;
        virtual int GetTarget() const = 0;
    };
} // namespace datamanagement

#endif