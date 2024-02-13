#include "DataTable.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>

namespace Data {
    int DataTableShape::operator[](int index) const {
        if (index != 0 && index != 1) {
            throw std::runtime_error("Invalid data shape. Valid indices "
                                     "are 0 for rows and 1 for columns.");
        }
        return index == 0 ? nrows : ncols;
    }

    DataTable::DataTable(const std::string &filename, bool hasHeaders,
                         char delim) {
        if (!this->fromCSV(filename, hasHeaders, delim)) {
            throw std::invalid_argument("File " + filename +
                                        " could not be found!");
        }
    }

    DataTable::DataTable(const std::string &dbfile,
                         const std::string &tablename) {
        throw std::logic_error("Not Implemented Yet");
    }

    DataTable::DataTable(std::map<std::string, std::vector<std::string>> data,
                         DataTableShape shape,
                         std::vector<std::string> headOrder) {
        this->data = data;
        this->shape = shape;
        if (headOrder.empty() || headOrder.size() != data.size()) {
            for (auto kv : this->data) {
                this->headerOrder.push_back(kv.first);
            }
        } else {
            this->headerOrder = headOrder;
        }
    }

    DataTable::DataTable(const DataTable &dt) {
        std::map<std::string, std::vector<std::string>> data =
            dt.getDataAsMap();
        DataTableShape shape = dt.getShape();
        std::vector<std::string> headOrder = dt.getHeaders();
        this->data = data;
        this->shape = shape;
        if (headOrder.empty() || headOrder.size() != data.size()) {
            for (auto kv : this->data) {
                this->headerOrder.push_back(kv.first);
            }
        } else {
            this->headerOrder = headOrder;
        }
    }

    std::vector<std::string> DataTable::loadRows(std::ifstream &csvStream) {
        std::vector<std::string> contents = {};
        std::string line;
        while (std::getline(csvStream, line)) {
            contents.push_back(line);
        }
        return contents;
    }

    bool DataTable::fromCSV(const std::string &filename, bool hasHeaders,
                            char delim) {
        std::ifstream csvFile(filename);
        if (!csvFile) {
            return false;
        }

        std::vector<std::string> contents = this->loadRows(csvFile);
        csvFile.close();

        if (contents.empty()) {
            return false;
        }

        std::vector<std::string> firstLine = this->split(contents[0], delim);
        this->shape.setNCols(firstLine.size());

        if (!hasHeaders) {
            for (int i = 0; i < firstLine.size(); ++i) {
                this->data[std::to_string(i)] = {};
                this->headerOrder.push_back(std::to_string(i));
            }
        } else {
            for (std::string header : firstLine) {
                header.erase(std::remove(header.begin(), header.end(), '\"'),
                             header.end());
                this->data[header] = {};
                this->headerOrder.push_back(header);
            }
            // remove the headers from the contents
            contents.erase(contents.begin());
        }

        for (std::string line : contents) {
            std::vector<std::string> rowData = this->split(line, delim);
            for (int i = 0; i < rowData.size(); ++i) {
                this->data[this->headerOrder[i]].push_back(rowData[i]);
            }
            this->shape.setNRows(this->shape.getNRows() + 1);
        }

        return true;
    }

    bool DataTable::fromSQL(const std::string &dbfile,
                            const std::string &tablename) {
        throw std::logic_error("Not Implemented Yet");
    }

    void DataTable::toCSV(const std::string &filename) const {
        std::ofstream csvFile(filename);
        if (!csvFile) {
            throw std::runtime_error("Error: Could notopen file '" + filename +
                                     "'.");
        }

        for (std::string head : this->headerOrder) {
            csvFile << head << ",";
        }
        csvFile << std::endl;

        for (int rowIdx = 0; rowIdx < this->shape.getNRows(); ++rowIdx) {
            for (std::string head : this->headerOrder) {
                csvFile << this->data.at(head)[rowIdx] << ",";
            }
            csvFile << std::endl;
        }
        csvFile.close();
    }

    std::shared_ptr<IDataTable> DataTable::getRow(int row) const {
        rowErrorCheck(row);
        std::map<std::string, std::vector<std::string>> newData;
        for (auto kv : this->data) {
            newData[kv.first] = {kv.second[row]};
        }
        DataTableShape newShape(1, this->shape.getNCols());
        std::shared_ptr<IDataTable> newDT =
            std::make_shared<DataTable>(newData, newShape, this->headerOrder);
        return newDT;
    }

    std::vector<std::string> DataTable::getColumnNames() const {
        std::vector<std::string> columnVec;
        for (auto kv : this->data) {
            columnVec.push_back(kv.first);
        }
        return columnVec;
    }

    std::shared_ptr<IDataTable> DataTable::operator[](int idx) const {
        return this->getRow(idx);
    }

    std::vector<std::string>
    DataTable::operator[](std::string columnName) const {
        return this->getColumn(columnName);
    }

    std::vector<std::string>
    DataTable::getColumn(std::string columnName) const {
        columnErrorCheck(columnName);
        return this->data.at(columnName);
    }

    bool DataTable::checkColumnExists(std::string columnName) const {
        std::vector<std::string> headers = this->getHeaders();
        for (std::string header : headers) {
            if (header.compare(columnName) == 0) {
                return true;
            }
        }
        return false;
    }

    std::shared_ptr<IDataTable>
    DataTable::selectColumns(std::vector<std::string> columnNames) const {
        if (columnNames.empty()) {
            return {};
        }
        std::map<std::string, std::vector<std::string>> newData;
        for (std::string colName : columnNames) {
            columnErrorCheck(colName);
            newData[colName] = this->data.at(colName);
        }
        DataTableShape newShape(newData[columnNames[0]].size(),
                                columnNames.size());
        std::shared_ptr<IDataTable> newDT =
            std::make_shared<DataTable>(newData, newShape, columnNames);
        return newDT;
    }

    std::shared_ptr<IDataTable>
    DataTable::selectRows(std::vector<int> idxs) const {
        std::map<std::string, std::vector<std::string>> newData;
        for (auto kv : this->data) {
            newData[kv.first] = {};
            for (int idx : idxs) {
                rowErrorCheck(idx);
                newData[kv.first].push_back(kv.second[idx]);
            }
        }
        DataTableShape newShape(idxs.size(), this->data.size());
        std::shared_ptr<IDataTable> newDT =
            std::make_shared<DataTable>(newData, newShape, this->headerOrder);
        return newDT;
    }

    std::shared_ptr<IDataTable> DataTable::selectRowRange(int start,
                                                          int end) const {
        if (start < 0) {
            start = 0;
        }

        if (end < 0) {
            end = 0;
        } else if (end > this->nrows()) {
            end = this->nrows();
        }

        int total = end - start;
        if (total < 0) {
            return {};
        }

        rowErrorCheck(total);

        std::vector<int> idxs(total);
        std::iota(std::begin(idxs), std::end(idxs), start);
        return this->selectRows(idxs);
    }

    std::shared_ptr<IDataTable> DataTable::selectWhere(
        std::unordered_map<std::string, std::string> columnDataMap) const {
        std::unordered_map<int, int> rowIdxHits = {};
        for (auto kv : columnDataMap) {
            columnErrorCheck(kv.first);
            std::vector<int> hits = {};
            std::vector<std::string> column = getColumn(kv.first);
            for (int i = 0; i < column.size(); ++i) {
                if (kv.second == column[i]) {
                    if (rowIdxHits.find(i) != rowIdxHits.end()) {
                        rowIdxHits[i]++;
                    } else {
                        rowIdxHits[i] = 1;
                    }
                }
            }
        }
        std::vector<int> rowIdxs = {};
        for (auto kv : rowIdxHits) {
            if (kv.second == columnDataMap.size()) {
                rowIdxs.push_back(kv.first);
            }
        }
        std::sort(rowIdxs.begin(), rowIdxs.end());
        return selectRows(rowIdxs);
    }

    std::shared_ptr<IDataTable>
    DataTable::innerJoin(std::shared_ptr<IDataTable> const tableTwo,
                         std::string tableOneColumnName,
                         std::string tableTwoColumnName) const {
        std::vector<std::string> columnOne =
            this->getColumn(tableOneColumnName);
        std::vector<std::string> columnTwo =
            tableTwo->getColumn(tableTwoColumnName);

        std::vector<std::vector<int>> indices;
        for (int colOneIter = 0; colOneIter < columnOne.size(); ++colOneIter) {
            for (int colTwoIter = 0; colTwoIter < columnTwo.size();
                 ++colTwoIter) {
                if (columnOne[colOneIter] == columnTwo[colTwoIter]) {
                    indices.push_back({colOneIter, colTwoIter});
                }
            }
        }

        std::vector<std::string> t1headers = this->getHeaders();
        std::vector<std::string> t2headers = tableTwo->getHeaders();
        auto it = find(t2headers.begin(), t2headers.end(), tableTwoColumnName);
        t2headers.erase(it);

        // Add the t1 rows column by column
        std::map<std::string, std::vector<std::string>> newData;
        for (std::string t1header : t1headers) {
            std::vector<std::string> column = this->getColumn(t1header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[0]]);
            }
            newData[t1header] = newColumn;
        }

        // Add the t2 rows column by column
        for (std::string t2header : t2headers) {
            std::vector<std::string> column = tableTwo->getColumn(t2header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[1]]);
            }
            newData[t2header] = newColumn;
        }

        t1headers.insert(t1headers.end(), t2headers.begin(), t2headers.end());
        DataTableShape newShape(indices.size(), t1headers.size());
        std::shared_ptr<IDataTable> newDT =
            std::make_shared<DataTable>(newData, newShape, t1headers);
        return newDT;
    }

    std::shared_ptr<IDataTable>
    DataTable::innerJoin(std::shared_ptr<IDataTable> const tableTwo,
                         std::vector<std::string> tableOneColumnNames,
                         std::vector<std::string> tableTwoColumnNames) const {

        // throw error on invalid 1-1 join columns
        if (tableOneColumnNames.size() != tableTwoColumnNames.size()) {
            throw std::logic_error("Joining columns are not one-to-one.");
        }

        // return nothing if the column names are empty
        if (tableOneColumnNames.empty() || tableTwoColumnNames.empty()) {
            return {};
        }

        std::vector<std::vector<std::string>> t1Columns = {};
        for (std::string tableOneColumnName : tableOneColumnNames) {
            t1Columns.push_back(this->getColumn(tableOneColumnName));
        }

        std::vector<std::vector<std::string>> t2Columns = {};
        for (std::string tableTwoColumnName : tableTwoColumnNames) {
            t2Columns.push_back(tableTwo->getColumn(tableTwoColumnName));
        }

        // Should never be true if the getColumn fails and the ColumnNames
        // Params are not empty
        if (t1Columns.empty() || t2Columns.empty()) {
            throw std::logic_error("Joining columns not found in tables.");
        }

        std::vector<std::vector<int>> indices;
        for (int colOneIter = 0; colOneIter < t1Columns[0].size();
             ++colOneIter) {
            for (int colTwoIter = 0; colTwoIter < t2Columns[0].size();
                 ++colTwoIter) {
                bool flag = true;
                for (int tableIDIter = 0;
                     tableIDIter < tableOneColumnNames.size(); ++tableIDIter) {
                    if (t1Columns[tableIDIter][colOneIter] !=
                        t2Columns[tableIDIter][colTwoIter]) {
                        flag = false;
                    }
                }
                if (flag) {
                    indices.push_back({colOneIter, colTwoIter});
                }
            }
        }

        std::vector<std::string> t1headers = this->getHeaders();
        std::vector<std::string> t2headers = tableTwo->getHeaders();
        for (std::string tableTwoColumnName : tableTwoColumnNames) {
            auto it =
                find(t2headers.begin(), t2headers.end(), tableTwoColumnName);
            t2headers.erase(it);
        }

        // Add the t1 rows column by column
        std::map<std::string, std::vector<std::string>> newData;
        for (std::string t1header : t1headers) {
            std::vector<std::string> column = this->getColumn(t1header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[0]]);
            }
            newData[t1header] = newColumn;
        }

        // Add the t2 rows column by column
        for (std::string t2header : t2headers) {
            std::vector<std::string> column = tableTwo->getColumn(t2header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[1]]);
            }
            newData[t2header] = newColumn;
        }
        DataTableShape newShape(indices.size(),
                                t1headers.size() + t2headers.size());
        std::shared_ptr<IDataTable> newDT =
            std::make_shared<DataTable>(newData, newShape, this->headerOrder);
        return newDT;
    }

    std::shared_ptr<IDataTable>
    DataTable::leftJoin(std::shared_ptr<IDataTable> const tableTwo,
                        std::string tableOneColumnName,
                        std::string tableTwoColumnName) const {
        throw std::logic_error("Not Implemented Yet");
    }

    std::shared_ptr<IDataTable>
    DataTable::rightJoin(std::shared_ptr<IDataTable> const tableTwo,
                         std::string tableOneColumnName,
                         std::string tableTwoColumnName) const {
        throw std::logic_error("Not Implemented Yet");
    }

    std::shared_ptr<IDataTable>
    DataTable::outerJoin(std::shared_ptr<IDataTable> const tableTwo,
                         std::string tableOneColumnName,
                         std::string tableTwoColumnName) const {
        throw std::logic_error("Not Implemented Yet");
    }

    void DataTable::dropColumns(std::vector<std::string> columnNames) {
        for (std::string col : columnNames) {
            this->dropColumn(col);
        }
    }

    void DataTable::dropColumn(std::string column) {
        try {
            columnErrorCheck(column);
        } catch (std::logic_error ex) {
            throw ex;
        }

        this->data.erase(column);
        for (std::vector<std::string>::iterator it = this->headerOrder.begin();
             it != this->headerOrder.end(); ++it) {
            if (*it == column) {
                this->headerOrder.erase(it);
                break;
            }
        }
    }

    // TODO Implement Shuffling
    void DataTable::shuffleRows(int seed) {
        // auto rng = std::default_random_engine{};
        // rng.seed(seed);

        // std::vector<int> order(this->data.begin()->second.size());
        // std::iota(std::begin(order), std::end(order), 1);
        // std::shuffle(std::begin(order), std::end(order), rng);

        //
        // //
        // https://stackoverflow.com/questions/236172/how-do-i-sort-a-stdvector-by-the-values-of-a-different-stdvector
        // // Probably want to speed this up

        // for (auto &kv : this->data) {
        //     for (int s = 0, d; s < order.size(); ++s) {
        //         for (d = order[s]; d < s; d = order[d])
        //             ;
        //         if (d == s) {
        //             while (d = order[d], d != s) {
        //                 std::swap(kv.second[s], kv.second[d]);
        //             }
        //         }
        //     }
        // }

        throw std::logic_error("Not Implemented Yet");
    }

    std::string DataTable::min(std::string column) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(column));

        std::vector<int>::iterator minElement =
            std::min_element(std::begin(colData), std::end(colData));

        return std::to_string(*minElement);
    }

    std::string DataTable::max(std::string column) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(column));

        std::vector<int>::iterator maxElement =
            std::max_element(std::begin(colData), std::end(colData));

        return std::to_string(*maxElement);
    }

    std::string DataTable::min() const {
        std::vector<int> intData;
        for (auto kv : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(kv.second);
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
        for (auto kv : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(kv.second);
            std::vector<int>::iterator maxElement =
                std::max_element(std::begin(tempData), std::end(tempData));
            intData.push_back(*maxElement);
        }

        std::vector<int>::iterator maxElement =
            std::max_element(std::begin(intData), std::end(intData));
        return std::to_string(*maxElement);
    }

    double DataTable::sum(std::string column) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(column));
        double result = std::reduce(colData.begin(), colData.end());
        return result;
    }

    double DataTable::sum() const {
        std::vector<int> intData;
        double runningSum = 0;
        for (auto kv : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(kv.second);
            runningSum += std::reduce(tempData.begin(), tempData.end());
        }
        return runningSum;
    }

    double DataTable::mean(std::string column) const {
        std::vector<int> colData =
            this->convertStringVecToInt(this->getColumn(column));
        double result = std::reduce(colData.begin(), colData.end()) /
                        ((double)(colData.size()));
        return result;
    }

    double DataTable::mean() const {
        std::vector<int> intData;
        double runningSum = 0;
        double count = 0;
        for (auto kv : this->data) {
            std::vector<int> tempData = this->convertStringVecToInt(kv.second);
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

    std::shared_ptr<IDataTable>
    DataTable::concat(IDataTable const &tableTwo) const {
        std::map<std::string, std::vector<std::string>> newData = {};
        for (auto kv : this->data) {
            std::vector<std::string> tempVec;
            std::vector<std::string> tableTwoVec = tableTwo[kv.first];
            tempVec.reserve(kv.second.size() + tableTwoVec.size());
            tempVec.insert(tempVec.end(), kv.second.begin(), kv.second.end());
            tempVec.insert(tempVec.end(), tableTwoVec.begin(),
                           tableTwoVec.end());
            newData[kv.first] = tempVec;
        }
        DataTableShape newShape(this->nrows() + tableTwo.nrows(),
                                this->ncols());

        std::shared_ptr<IDataTable> newDT =
            std::make_shared<DataTable>(newData, newShape, this->headerOrder);
        return newDT;
    }

} // namespace Data