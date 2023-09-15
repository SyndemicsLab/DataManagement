#include "DataTable.hpp"

namespace Data {
    DataTable::DataTable(const std::string &filename, bool hasHeaders = true,
                         char delim = ',') {
        this->fromCSV(filename, hasHeaders, delim);
    }

    DataTable::DataTable(const std::string &dbfile,
                         const std::string &tablename) {}

    bool DataTable::fromCSV(const std::string &filename, bool hasHeaders = true,
                            char delim = ',') {
        std::ifstream csvFile(filename);
        if (!csvFile) {
            throw std::runtime_error("ERROR: Unable to open file '" + filename +
                                     "'.");
        }

        if (hasHeaders) {
            std::string firstLine;
            std::getline(csvFile, firstLine);
            this->headers = split(firstLine, delim);
            this->shape.setNCols(this->headers.size());
        } else {
            this->headers = {""};
        }

        std::vector<std::string> content;
        std::string dataLine;
        while (std::getline(csvFile, dataLine)) {
            content.push_back(dataLine);
        }

        for (std::string line : content) {
            std::vector<std::string> rowData = split(line, delim);

            // This should only ever execute the first time when hasHeaders is
            // false
            if (!hasHeaders && this->shape.getNCols() == 0) {
                this->shape.setNCols(rowData.size());
            }
            if (rowData.size() != this->shape.getNCols()) {
                continue; // do not add this row to the data as the number of
                          // columns is off
            }
            this->data.push_back(rowData);
            this->shape.setNRows(this->shape.getNRows() + 1);
        }
    }
} // namespace Data