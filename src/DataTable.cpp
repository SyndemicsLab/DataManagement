#include "DataTable.hpp"

namespace Data {
    DataTable::DataTable(const std::string &filename, bool hasHeaders,
                         char delim) {
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

    bool DataTable::fromCSV(const std::string &filename, bool hasHeaders,
                            char delim) {
        std::ifstream csvFile(filename);
        if (!csvFile) {
            return false;
        }

        if (hasHeaders) {
            std::string firstLine;
            std::getline(csvFile, firstLine);
            this->headers = this->split(firstLine, delim);
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
            std::vector<std::string> rowData = this->split(line, delim);

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

    bool DataTable::fromSQL(const std::string &dbfile,
                            const std::string &tablename) {
        return false;
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

    std::vector<std::string> DataTable::getColumn(int idx) const {
        if (idx >= this->headers.size() && this->getData().size() > 0 &&
            idx >= this->getData()[0].size()) {
            return {}; // not found
        }
        std::vector<std::string> result;
        result.push_back(this->headers[idx]);
        for (std::vector<std::string> row : this->data) {
            result.push_back(row[idx]);
        }
        return result;
    }

    DataTable DataTable::selectColumns(std::vector<int> columnIdxs) const {
        std::vector<int> sortedIdxs(columnIdxs.size());
        std::partial_sort_copy(std::begin(columnIdxs), std::end(columnIdxs),
                               std::begin(sortedIdxs), std::end(sortedIdxs));

        int lastIdx = sortedIdxs[sortedIdxs.size() - 1];
        if (lastIdx >= this->headers.size() && this->getData().size() > 0 &&
            lastIdx >= this->getData()[0].size()) {
            return {}; // not found
        }

        std::vector<std::string> newDTHeaders;
        for (int sIdx : sortedIdxs) {
            newDTHeaders.push_back(this->headers[sIdx]);
        }

        std::vector<std::vector<std::string>> newDTData;
        for (std::vector<std::string> row : this->data) {
            std::vector<std::string> newRowData;
            for (int sIdx : sortedIdxs) {
                newRowData.push_back(row[sIdx]);
            }
            newDTData.push_back(newRowData);
        }

        DataTableShape newDTShape;
        newDTShape.setNCols(newDTData[0].size());
        newDTShape.setNRows(newDTData.size());

        DataTable newDT(newDTData, newDTHeaders, newDTShape);
        return newDT;
    }

    DataTable
    DataTable::selectColumns(std::vector<std::string> columnNames) const {
        std::vector<std::vector<std::string>> columnDataVectors;
        std::vector<std::string> headers = columnNames;
        int counter = 0;
        for (std::string col : columnNames) {
            std::vector<std::string> d = this->getColumn(col);
            if (!d.empty()) {
                columnDataVectors.push_back(d);
            } else {
                headers.erase(headers.begin() + counter);
                counter--;
            }
            counter++;
        }
        std::vector<std::vector<std::string>> newDTData(
            columnDataVectors[0].size());
        int i = 0, j = 0;
        for (std::vector<std::string> columnData : columnDataVectors) {
            for (std::string data : columnData) {
                newDTData[i][j] = data;
                i++;
            }
            j++;
        }
        DataTableShape newDTShape;
        newDTShape.setNCols(newDTData[0].size());
        newDTShape.setNRows(newDTData.size());
        DataTable newDT(newDTData, headers, newDTShape);
        return newDT;
    }

    DataTable DataTable::selectRows(std::vector<int> idxs) const {
        std::vector<int> sortedIdxs(idxs.size());
        std::partial_sort_copy(std::begin(idxs), std::end(idxs),
                               std::begin(sortedIdxs), std::end(sortedIdxs));
        std::vector<std::vector<std::string>> newDTData;
        for (int idx : idxs) {
            newDTData.push_back(this->data[idx]);
        }
        DataTableShape newDTShape;
        newDTShape.setNCols(newDTData[0].size());
        newDTShape.setNRows(newDTData.size());
        DataTable newDT(newDTData, this->headers, newDTShape);
        return newDT;
    }

    DataTable DataTable::selectRowRange(int start, int end) const {
        int total = end - start;
        if (total < 0) {
            return {};
        }
        std::vector<int> idxs(total);
        std::iota(std::begin(idxs), std::end(idxs), start);
        return this->selectRows(idxs);
    }

    void DataTable::dropColumns(std::vector<int> columns) {
        std::vector<int> sortedIdxs(columns.size());
        std::partial_sort_copy(std::begin(columns), std::end(columns),
                               std::begin(sortedIdxs), std::end(sortedIdxs));

        int offset = 0;
        for (int idx : sortedIdxs) {
            if (this->headers.size() > 0) {
                this->headers.erase(this->headers.begin() + idx - offset);
            }
            this->data.erase(this->data.begin() + idx - offset);
            offset++;
        }
    }

    void DataTable::dropColumns(std::vector<std::string> columnNames) {
        std::vector<int> colIdxs;
        for (std::string col : columnNames) {
            auto iter =
                std::find(this->headers.begin(), this->headers.end(), col);
            if (iter != this->headers.end()) {
                colIdxs.push_back(iter - this->headers.begin());
            }
        }
        this->dropColumns(colIdxs);
    }

    void DataTable::dropColumn(int column) {
        this->headers.erase(this->headers.begin() + column);
        this->data.erase(this->data.begin() + column);
    }

    void DataTable::dropColumn(std::string column) {
        int idx = 0;
        for (std::string colName : this->headers) {
            if (column == colName) {
                break;
            }
            idx++;
        }
        this->dropColumn(idx);
    }

    void DataTable::shuffleRows(int seed) {
        auto rng = std::default_random_engine{};
        rng.seed(seed);
        std::shuffle(std::begin(this->data), std::end(this->data), rng);
    }

    std::string DataTable::min(int col) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(col));

        std::vector<int>::iterator minElement =
            std::min_element(std::begin(colData), std::end(colData));

        return std::to_string(*minElement);
    }

    std::string DataTable::max(int col) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(col));

        std::vector<int>::iterator maxElement =
            std::max_element(std::begin(colData), std::end(colData));

        return std::to_string(*maxElement);
    }

    std::string DataTable::min() const {
        std::vector<int> intData;
        for (std::vector<std::string> col : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(col);
            std::vector<int>::iterator minElement =
                std::min_element(std::begin(tempData), std::end(tempData));
            intData.push_back(*minElement);
        }

        std::vector<int>::iterator minElement =
            std::min_element(std::begin(intData), std::end(intData));
        return std::to_string(*minElement);
    }

    std::string DataTable::max() const {
        std::vector<int> intData;
        for (std::vector<std::string> col : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(col);
            std::vector<int>::iterator maxElement =
                std::max_element(std::begin(tempData), std::end(tempData));
            intData.push_back(*maxElement);
        }

        std::vector<int>::iterator maxElement =
            std::max_element(std::begin(intData), std::end(intData));
        return std::to_string(*maxElement);
    }

    double DataTable::sum(int col) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(col));
        double result = std::reduce(colData.begin(), colData.end());
        return result;
    }

    double DataTable::sum() const {
        std::vector<int> intData;
        double runningSum = 0;
        for (std::vector<std::string> col : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(col);
            runningSum += std::reduce(tempData.begin(), tempData.end());
        }
        return runningSum;
    }

    double DataTable::mean(int col) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(col));
        double result = std::reduce(colData.begin(), colData.end()) /
                        ((double)(colData.size()));
        return result;
    }

    double DataTable::mean() const {
        std::vector<int> intData;
        double runningSum = 0;
        double count = 0;
        for (std::vector<std::string> col : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(col);
            runningSum += std::reduce(tempData.begin(), tempData.end());
            count += tempData.size();
        }
        return runningSum / count;
    }

    std::vector<int>
    DataTable::convertStringVecToInt(std::vector<std::string> data) const {
        std::vector<int> intData;
        std::transform(data.begin(), data.end(), std::back_inserter(intData),
                       [](const std::string &str) { return std::stoi(str); });
        return intData;
    }

    std::ostream &operator<<(std::ostream &os, const DataTable &table) {
        if (!table.getData().empty() || (table.getShape()[1] == 0))
            return os;

        std::string rowBuffer;
        if (!table.getHeaders().empty()) {
            std::vector<std::string> headers = table.getHeaders();
            for (std::string header : headers) {
                rowBuffer += header + ",";
            }

            rowBuffer.pop_back();
            os << rowBuffer << "\n";
        }

        std::vector<std::vector<std::string>> tempData = table.getData();
        for (std::vector<std::string> iData : tempData) {
            rowBuffer = "";
            for (std::string jData : iData) {
                rowBuffer += jData + ",";
            }
            rowBuffer.pop_back();
            os << rowBuffer << "\n";
        }
        return os;
    }

} // namespace Data