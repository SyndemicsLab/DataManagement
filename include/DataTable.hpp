#ifndef DATATABLE_HPP_
#define DATATABLE_HPP_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Built in part on the back of:
// https://github.com/anthonymorast/DataTables/tree/master

namespace Data {

    class DataTableShape {
    private:
        int nrows = 0;
        int ncols = 0;

    public:
        DataTableShape(){};
        DataTableShape(int nrows, int ncols) {
            this->nrows = nrows;
            this->ncols = ncols;
        };
        int getNRows() const { return this->nrows; }
        int getNCols() const { return this->ncols; }
        void setNRows(int nrows) { this->nrows = nrows; }
        void setNCols(int ncols) { this->ncols = ncols; }
        friend std::ostream &operator<<(std::ostream &os,
                                        const DataTableShape shape) {
            os << "(" << shape.nrows << ", " << shape.ncols << ")";
            return os;
        }

        int operator[](int index) const {
            if (index != 0 && index != 1) {
                throw std::runtime_error("Invalid data shape. Valid indices "
                                         "are 0 for rows and 1 for columns.");
            }
            return index == 0 ? nrows : ncols;
        }
    };

    class DataTable {
    private:
        std::map<std::string, std::vector<std::string>> data;
        DataTableShape shape;
        std::vector<int>
        convertStringVecToInt(std::vector<std::string> data) const;

        std::vector<std::string> loadData(std::ifstream &csvStream);
        // int getIdxOfColumnName(std::string columnName) const;

        template <typename T>
        void split(const std::string &s, char delim, T result) {
            std::istringstream iss(s);
            std::string item;
            while (std::getline(iss, item, delim)) {
                *result++ = item;
            }
        };

        std::vector<std::string> split(const std::string &s, char delim) {
            std::vector<std::string> elems;
            split(s, delim, std::back_inserter(elems));
            return elems;
        };

    public:
        DataTable(){};
        DataTable(const std::string &filename, bool hasHeaders = true,
                  char delim = ',');
        DataTable(const std::string &dbfile, const std::string &tablename);
        DataTable(std::map<std::string, std::vector<std::string>> data,
                  DataTableShape shape);
        ~DataTable(){};
        void toCSV(const std::string &filename) const;
        bool fromCSV(const std::string &filename, bool hasHeaders = true,
                     char delim = ',');
        bool fromSQL(const std::string &dbfile, const std::string &tablename);

        std::vector<std::vector<std::string>> getData() const {
            std::vector<std::vector<std::string>> dat;
            for (auto kv : this->data) {
                dat.push_back(kv.second);
            }
            return dat;
        }

        std::vector<std::string> getRow(int idx) const;
        std::vector<std::string> getColumn(std::string columnName) const;
        DataTable selectColumns(std::vector<std::string> columnNames) const;
        DataTable selectRows(std::vector<int> idxs) const;
        DataTable selectRowRange(int start, int end) const;
        DataTable selectWhere(
            std::unordered_map<std::string, std::string> columnDataMap) const;

        DataTable innerJoin(DataTable const &tableTwo,
                            std::string tableOneColumnName,
                            std::string tableTwoColumnName) const;

        DataTable innerJoin(DataTable const &tableTwo,
                            std::vector<std::string> tableOneColumnNames,
                            std::vector<std::string> tableTwoColumnNames) const;

        DataTable leftJoin(DataTable const &tableTwo,
                           std::string tableOneColumnName,
                           std::string tableTwoColumnName) const;

        DataTable rightJoin(DataTable const &tableTwo,
                            std::string tableOneColumnName,
                            std::string tableTwoColumnName) const;

        DataTable outerJoin(DataTable const &tableTwo,
                            std::string tableOneColumnName,
                            std::string tableTwoColumnName) const;

        DataTable topNRows(int n) const { return this->selectRowRange(0, n); }
        DataTable bottomNRows(int n) const {
            return selectRowRange(shape[0] - n, shape[0]);
        }
        DataTable head() const { return topNRows(10); }
        DataTable tail() const { return bottomNRows(10); }

        std::vector<std::string> getHeaders() const {
            std::vector<std::string> headers;
            for (auto kv : this->data) {
                headers.push_back(kv.first);
            }
            return headers;
        }

        // table operations
        void dropColumns(std::vector<std::string> column_names);
        void dropColumn(std::string column);
        void shuffleRows(int seed = 0);

        std::string min(std::string column) const;
        std::string max(std::string column) const;
        std::string min() const;
        std::string max() const;
        double sum(std::string column) const;
        double sum() const;
        double mean(std::string column) const;
        double mean() const;

        // overridden operators
        std::vector<std::string> operator[](int idx) const; // select row
        std::vector<std::string>
        operator[](std::string columnName) const; // select column into DT
        friend std::ostream &operator<<(std::ostream &os,
                                        const DataTable &table);

        // for other classes
        int nrows() const { return shape[0]; }
        int ncols() const { return shape[1]; }
        DataTableShape getShape() const { return shape; }
    };
} // namespace Data

#endif