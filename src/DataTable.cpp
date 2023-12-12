#include "DataTable.hpp"

namespace Data {
    DataTable::DataTable(const std::string &filename, bool hasHeaders,
                         char delim) {
        this->fromCSV(filename, hasHeaders, delim);
    }

    DataTable::DataTable(const std::string &dbfile,
                         const std::string &tablename) {}

    DataTable::DataTable(std::map<std::string, std::vector<std::string>> data,
                         DataTableShape shape) {
        this->data = data;
        this->shape = shape;
    }

    std::vector<std::string> DataTable::loadRows(std::ifstream &csvStream) {
        std::vector<std::string> contents;
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

        std::vector<std::string> firstLine = this->split(contents[0], delim);
        this->shape.setNCols(firstLine.size());

        std::vector<std::string> headerOrder;
        if (!hasHeaders) {
            for (int i = 0; i < firstLine.size(); ++i) {
                this->data[std::to_string(i)] = {};
                headerOrder.push_back(std::to_string(i));
            }
        } else {
            for (std::string header : firstLine) {
                this->data[header] = {};
                headerOrder.push_back(header);
            }
            // remove the headers from the contents
            contents.erase(contents.begin());
        }

        for (std::string line : contents) {
            std::vector<std::string> rowData = this->split(line, delim);
            for (int i = 0; i < rowData.size(); ++i) {
                this->data[headerOrder[i]].push_back(rowData[i]);
            }
            this->shape.setNRows(this->shape.getNRows() + 1);
        }

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

        std::vector<std::vector<std::string>> contents;
        for (auto kv : this->data) {
            csvFile << kv.first << ",";
            contents.push_back(kv.second);
        }
        csvFile << std::endl;

        for (int colIdx = 0; colIdx < this->shape.getNCols(); ++colIdx) {
            for (int rowIdx = 0; rowIdx < this->shape.getNRows(); ++rowIdx) {
                csvFile << contents[colIdx][rowIdx] << ",";
            }
            csvFile << std::endl;
        }
        csvFile.close();
    }

    std::vector<std::string> DataTable::getRow(int row) const {
        std::vector<std::string> ret;
        for (auto kv : this->data) {
            ret.push_back(kv.second[row]);
        }
        return ret;
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
        return this->data.at(columnName);
    }

    DataTable
    DataTable::selectColumns(std::vector<std::string> columnNames) const {
        std::map<std::string, std::vector<std::string>> newData;
        for (std::string colName : columnNames) {
            newData[colName] = this->data.at(colName);
        }
        DataTableShape newShape(newData[columnNames[0]].size(),
                                columnNames.size());
        DataTable newDT(newData, newShape);
        return newDT;
    }

    DataTable DataTable::selectRows(std::vector<int> idxs) const {
        std::map<std::string, std::vector<std::string>> newData;
        for (auto kv : this->data) {
            if (idxs.size() > kv.second.size()) {
                DataTable newDT(this->data, this->shape);
                return newDT;
            }
            newData[kv.first] = {};
            for (int idx : idxs) {
                newData[kv.first].push_back(kv.second[idx]);
            }
        }
        DataTableShape newShape(idxs.size(), this->data.size());
        DataTable newDT(newData, newShape);
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

    DataTable DataTable::selectWhere(
        std::unordered_map<std::string, std::string> columnDataMap) const {
        return {};
    }

    DataTable DataTable::innerJoin(DataTable const &tableTwo,
                                   std::string tableOneColumnName,
                                   std::string tableTwoColumnName) const {
        std::vector<std::string> columnOne = this->data.at(tableOneColumnName);
        std::vector<std::string> columnTwo =
            tableTwo.getColumn(tableTwoColumnName);

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
        std::vector<std::string> t2headers = tableTwo.getHeaders();
        auto it = find(t2headers.begin(), t2headers.end(), tableTwoColumnName);
        t2headers.erase(it);

        // Add the t1 rows column by column
        std::map<std::string, std::vector<std::string>> newData;
        for (std::string t1header : t1headers) {
            std::vector<std::string> column = this->data.at(t1header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[0]]);
            }
            newData[t1header] = newColumn;
        }

        // Add the t2 rows column by column
        for (std::string t2header : t2headers) {
            std::vector<std::string> column = tableTwo.getColumn(t2header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[1]]);
            }
            newData[t2header] = newColumn;
        }
        DataTableShape newShape(indices.size(),
                                t1headers.size() + t2headers.size());
        DataTable newDT(newData, newShape);
        return newDT;
    }

    DataTable
    DataTable::innerJoin(DataTable const &tableTwo,
                         std::vector<std::string> tableOneColumnNames,
                         std::vector<std::string> tableTwoColumnNames) const {

        if (tableOneColumnNames.size() != tableTwoColumnNames.size()) {
            // Invalid table comparisons
            return {};
        }

        std::vector<std::vector<std::string>> t1Columns;
        for (std::string tableOneColumnName : tableOneColumnNames) {
            t1Columns.push_back(this->data.at(tableOneColumnName));
        }

        std::vector<std::vector<std::string>> t2Columns;
        for (std::string tableTwoColumnName : tableTwoColumnNames) {
            t2Columns.push_back(this->data.at(tableTwoColumnName));
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
        std::vector<std::string> t2headers = tableTwo.getHeaders();
        for (std::string tableTwoColumnName : tableTwoColumnNames) {
            auto it =
                find(t2headers.begin(), t2headers.end(), tableTwoColumnName);
            t2headers.erase(it);
        }

        // Add the t1 rows column by column
        std::map<std::string, std::vector<std::string>> newData;
        for (std::string t1header : t1headers) {
            std::vector<std::string> column = this->data.at(t1header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[0]]);
            }
            newData[t1header] = newColumn;
        }

        // Add the t2 rows column by column
        for (std::string t2header : t2headers) {
            std::vector<std::string> column = tableTwo.getColumn(t2header);
            std::vector<std::string> newColumn;
            for (std::vector<int> idxs : indices) {
                newColumn.push_back(column[idxs[1]]);
            }
            newData[t2header] = newColumn;
        }
        DataTableShape newShape(indices.size(),
                                t1headers.size() + t2headers.size());
        DataTable newDT(newData, newShape);
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

    void DataTable::dropColumns(std::vector<std::string> columnNames) {
        for (std::string col : columnNames) {
            this->data.erase(col);
        }
    }

    void DataTable::dropColumn(std::string column) { this->data.erase(column); }

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

} // namespace Data