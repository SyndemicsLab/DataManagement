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
        // result.push_back(columnName);
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
        // result.push_back(this->headers[idx]);
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
        std::vector<int> colIdxs;
        for (std::string colname : columnNames) {
            int idx = this->getIdxOfColumnName(colname);
            if (idx != -1) {
                colIdxs.push_back(idx);
            }
        }
        return this->selectColumns(colIdxs);
    }

    DataTable DataTable::selectRows(std::vector<int> idxs) const {
        std::vector<int> sortedIdxs(idxs.size());
        std::partial_sort_copy(std::begin(idxs), std::end(idxs),
                               std::begin(sortedIdxs), std::end(sortedIdxs));
        std::vector<std::vector<std::string>> newDTData;
        for (int idx : idxs) {
            if (idx < this->data.size()) {
                newDTData.push_back(this->data[idx]);
            }
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

    DataTable DataTable::innerJoin(DataTable const &tableTwo,
                                   std::string tableOneColumnName,
                                   std::string tableTwoColumnName) const {
        int t1ColIdx = -1, t2ColIdx = -1;
        std::vector<std::string> headerVec;
        this->checkJoin(t1ColIdx, tableOneColumnName, tableTwo, t2ColIdx,
                        tableTwoColumnName, headerVec);
        if (t1ColIdx < 0 || t2ColIdx < 0) {
            return {};
        }

        std::vector<std::vector<std::string>> newDTData;
        for (int i = 0; i < shape.getNRows(); ++i) {
            for (int j = 0; j < tableTwo.shape.getNRows(); ++j) {
                std::vector<std::string> t1row = getRow(i);
                std::vector<std::string> t2row = tableTwo.getRow(j);
                if (t1row[t1ColIdx] == t2row[t2ColIdx]) {
                    t2row.erase(t2row.begin() + t2ColIdx);
                    t1row.insert(t1row.end(), t2row.begin(), t2row.end());
                    newDTData.push_back(t1row);
                }
            }
        }

        DataTableShape newDTShape;
        newDTShape.setNCols(headerVec.size());
        newDTShape.setNRows(newDTData.size());

        DataTable newDT(newDTData, headerVec, newDTShape);

        return newDT;
    }

    DataTable
    DataTable::innerJoin(DataTable const &tableTwo,
                         std::vector<std::string> tableOneColumnNames,
                         std::vector<std::string> tableTwoColumnNames) const {
        std::vector<int> t1ColIndices;
        std::vector<int> t2ColIndices;
        std::vector<std::string> headerVec;
        this->checkJoin(t1ColIndices, tableOneColumnNames, tableTwo,
                        t2ColIndices, tableTwoColumnNames, headerVec);

        if (t1ColIndices.size() != t2ColIndices.size()) {
            return {};
        }
        for (int i = 0; i < t1ColIndices.size(); ++i) {
            if (t1ColIndices[i] < 0 || t1ColIndices[i] < 0) {
                return {};
            }
        }

        std::vector<std::vector<std::string>> newDTData;
        for (int i = 0; i < shape.getNRows(); ++i) {
            for (int j = 0; j < tableTwo.shape.getNRows(); ++j) {
                std::vector<std::string> t1row = getRow(i);
                std::vector<std::string> t2row = tableTwo.getRow(j);
                bool match = true;
                for (int k = 0; k < t1ColIndices.size(); ++k) {
                    if (t1row[k] != t2row[k]) {
                        match = false;
                    }
                }
                if (match) {
                    // reverse iterator so we don't have to worry about erasing
                    // idxs
                    for (std::vector<int>::reverse_iterator rIt =
                             t2ColIndices.rbegin();
                         rIt != t2ColIndices.rend(); ++rIt) {
                        int idx =
                            std::distance(begin(t2ColIndices), rIt.base()) - 1;
                        t2row.erase(t2row.begin() + idx);
                    }
                    t1row.insert(t1row.end(), t2row.begin(), t2row.end());
                    newDTData.push_back(t1row);
                }
            }
        }

        DataTableShape newDTShape;
        newDTShape.setNCols(headerVec.size());
        newDTShape.setNRows(newDTData.size());

        DataTable newDT(newDTData, headerVec, newDTShape);

        return newDT;
    }

    DataTable DataTable::leftJoin(DataTable const &tableTwo,
                                  std::string tableOneColumnName,
                                  std::string tableTwoColumnName) const {
        throw new std::logic_error("Not Implemented Yet");
    }

    DataTable DataTable::rightJoin(DataTable const &tableTwo,
                                   std::string tableOneColumnName,
                                   std::string tableTwoColumnName) const {
        throw new std::logic_error("Not Implemented Yet");
    }

    DataTable DataTable::outerJoin(DataTable const &tableTwo,
                                   std::string tableOneColumnName,
                                   std::string tableTwoColumnName) const {
        throw new std::logic_error("Not Implemented Yet");
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
            for (std::vector<std::string> &row : this->data) {
                row.erase(row.begin() + idx - offset);
            }
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
        for (std::vector<std::string> &row : this->data) {
            row.erase(row.begin() + column);
        }
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

    int DataTable::getIdxOfColumnName(std::string columnName) const {
        int idx = 0;
        for (std::string colname : this->headers) {
            if (columnName == colname) {
                return idx;
            }
            idx++;
        }

        return -1;
    }

    void DataTable::checkJoin(int &t1ColIdx, std::string tableOneColumnName,
                              DataTable const &tableTwo, int &t2ColIdx,
                              std::string tableTwoColumnName,
                              std::vector<std::string> &headerVec) const {

        t1ColIdx = getIdxOfColumnName(tableOneColumnName);
        t2ColIdx = tableTwo.getIdxOfColumnName(tableTwoColumnName);
        if (t1ColIdx < 0 || t2ColIdx < 0) {
            return;
        }

        std::vector<std::string> temp = tableTwo.getHeaders();
        temp.erase(temp.begin() + t2ColIdx);

        std::vector<std::string> combinedHeaders = headers;
        combinedHeaders.insert(combinedHeaders.end(), temp.begin(), temp.end());

        headerVec = combinedHeaders;
    }

    void DataTable::checkJoin(std::vector<int> &t1ColIndices,
                              std::vector<std::string> tableOneColumnNames,
                              DataTable const &tableTwo,
                              std::vector<int> &t2ColIndices,
                              std::vector<std::string> tableTwoColumnNames,
                              std::vector<std::string> &headerVec) const {
        if (tableOneColumnNames.size() != tableTwoColumnNames.size()) {
            return;
        }
        for (int i = 0; i < tableTwoColumnNames.size(); ++i) {
            int t1ColIdx = getIdxOfColumnName(tableOneColumnNames[i]);
            int t2ColIdx = tableTwo.getIdxOfColumnName(tableTwoColumnNames[i]);
            if (t1ColIdx < 0 || t2ColIdx < 0) {
                return;
            }
            t1ColIndices.push_back(t1ColIdx);
            t2ColIndices.push_back(t2ColIdx);
        }

        std::vector<std::string> temp = tableTwo.getHeaders();

        for (std::vector<int>::reverse_iterator rIt = t2ColIndices.rbegin();
             rIt != t2ColIndices.rend(); ++rIt) {
            int idx = std::distance(begin(t2ColIndices), rIt.base()) - 1;
            temp.erase(temp.begin() + idx);
        }

        std::vector<std::string> combinedHeaders = headers;
        combinedHeaders.insert(combinedHeaders.end(), temp.begin(), temp.end());

        headerVec = combinedHeaders;
    }

} // namespace Data