#include "DataTable.hpp"

namespace Data {
    DataTable::DataTable(const std::string &filename, bool hasHeaders = true,
                         char delim = ',') {
        this->fromCSV(filename, hasHeaders, delim);
    }

    DataTable::DataTable(const std::string &dbfile,
                         const std::string &tablename) {}

    DataTable::DataTable(std::vector<std::vector<std::string>> data,
                         std::vector<std::string> headers,
                         DataTableShape shape) {
        this->headers = headers;
        this->data = data;
        this->shape = shape;
    }

    bool DataTable::fromCSV(const std::string &filename, bool hasHeaders = true,
                            char delim = ',') {
        std::ifstream csvFile(filename);
        if (!csvFile) {
            return false;
        }

        if (hasHeaders) {
            std::string firstLine;
            std::getline(csvFile, firstLine);
            this->headers = split(firstLine, delim);
            this->shape.setNCols(this->headers.size());
        } else {
            this->headers = {};
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
        csvFile.close();
        return true;
    }

    void DataTable::toCSV(const std::string &filename) const {
        std::ofstream csvFile(filename);
        if (!csvFile) {
            throw std::runtime_error("Error: Could notopen file '" + filename +
                                     "'.");
        }

        if (this->headers.size() > 0) {
            for (std::string header : this->headers) {
                csvFile << header << ",";
            }
            csvFile << std::endl;
        }

        for (std::vector<std::string> row : this->data) {
            for (std::string item : row) {
                csvFile << item << ",";
            }
            csvFile << std::endl;
        }
        csvFile.close();
    }

    std::vector<std::string> DataTable::getRow(int row) const {
        if (row > this->data.size() || row < 0) {
            return {};
        }
        return this->data[row];
    }

    std::vector<std::string> DataTable::operator[](int idx) const {
        return this->getRow(idx);
    }

    std::vector<std::string>
    DataTable::operator[](std::string columnName) const {
        return this->getColumn(columnName);
    }

    std::vector<std::string>
    DataTable::getColumn(std::string columnName) const {
        auto iter =
            std::find(this->headers.begin(), this->headers.end(), columnName);
        if (iter == this->headers.end()) {
            return {}; // not found
        }
        int idx = iter - this->headers.begin();
        std::vector<std::string> result;
        result.push_back(columnName);
        for (std::vector<std::string> row : this->data) {
            result.push_back(row[idx]);
        }
        return result;
    }

} // namespace Data