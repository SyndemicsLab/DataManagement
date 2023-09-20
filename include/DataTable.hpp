#ifndef DATATABLE_HPP_
#define DATATABLE_HPP_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// Built in part on the back of:
// https://github.com/anthonymorast/DataTables/tree/master

namespace Data {

    class DataTableShape {
    private:
        int nrows = 0;
        int ncols = 0;

    public:
        int getNRows() { return this->nrows; }
        int getNCols() { return this->ncols; }
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
        std::vector<std::string> headers;
        std::vector<std::vector<std::string>> data;
        DataTableShape shape;
        std::vector<int>
        convertStringVecToInt(std::vector<std::string> data) const;
        int getIdxOfColumnName(std::string columnName) const;

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
        DataTable(std::vector<std::vector<std::string>> data,
                  std::vector<std::string> headers, DataTableShape shape);
        ~DataTable(){};
        void toCSV(const std::string &filename) const;
        bool fromCSV(const std::string &filename, bool hasHeaders = true,
                     char delim = ',');
        bool fromSQL(const std::string &dbfile, const std::string &tablename);
        std::vector<std::vector<std::string>> getData() const { return data; }
        std::vector<std::string> getRow(int idx) const;
        std::vector<std::string> getColumn(int idx) const;
        std::vector<std::string> getColumn(std::string columnName) const;
        DataTable selectColumns(std::vector<int> columnIdxs) const;
        DataTable selectColumns(std::vector<std::string> columnNames) const;
        DataTable selectRows(std::vector<int> idxs) const;
        DataTable selectRowRange(int start, int end) const;
        DataTable topNRows(int n) const { return this->selectRowRange(0, n); }
        DataTable bottomNRows(int n) const {
            return selectRowRange(shape[0] - n, shape[0]);
        }
        DataTable head() const { return topNRows(10); }
        DataTable tail() const { return bottomNRows(10); }
        std::vector<std::string> getHeaders() const { return headers; }

        // table operations
        void dropColumns(std::vector<int> columns);
        void dropColumns(std::vector<std::string> column_names);
        void dropColumn(int column);
        void dropColumn(std::string column);
        void shuffleRows(int seed = 0);

        // data analysis
        // std::vector<std::string> pctChange(std::string column) const;
        // std::vector<std::string> pctChange(int column) const;
        // std::vector<std::string> sma(std::string column, int periods) const;
        // std::vector<std::string> sma(int column, int periods) const;
        // std::vector<std::string> ema(std::string column, int periods) const;
        // std::vector<std::string> ema(int column, int periods) const;
        // std::vector<std::string> rsi(std::string column,
        //                              int periods = 14) const;
        // std::vector<std::string> rsi(int column, int periods = 14) const;
        std::string min(int col) const;
        std::string max(int col) const;
        std::string min() const;
        std::string max() const;
        double sum(int col) const;
        double sum() const;
        double mean(int col) const;
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