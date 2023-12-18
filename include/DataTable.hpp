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

/// @brief Namespace defining Data Containers
namespace Data {

    /// @brief Helper Class used to hold DataTable Shapes
    class DataTableShape {
    private:
        int nrows = 0;
        int ncols = 0;

    public:
        /// @brief Default constructor. Leaves Rows and Cols set to 0.
        DataTableShape(){};

        /// @brief Primary use constructor. Provides a setter for Rows and Cols.
        /// @param nrows Number of Rows to provide to the shape
        /// @param ncols Number of Cols to provide to the shape
        DataTableShape(int nrows, int ncols) {
            if (nrows > 0 || ncols > 0) {
                this->nrows = nrows;
                this->ncols = ncols;
            }
        };

        /// @brief Getter for the Number of Rows
        /// @return Number of Rows as an Integer
        int getNRows() const { return this->nrows; }

        /// @brief Getter for Number of Columns
        /// @return Number of Columns as an Integer
        int getNCols() const { return this->ncols; }

        /// @brief Setter for Number of Rows
        /// @param nrows Number of Rows
        void setNRows(int nrows) { this->nrows = (nrows < 0) ? 0 : nrows; }

        /// @brief Setter for Number of Columns
        /// @param ncols Number of Columns
        void setNCols(int ncols) { this->ncols = (ncols < 0) ? 0 : ncols; }

        /// @brief Operator overload for outputting the shape
        /// @param os Stream to pass output to
        /// @param shape Class of DataTableShape to print
        /// @return stream with the shape printed
        friend std::ostream &operator<<(std::ostream &os,
                                        const DataTableShape shape) {
            os << "(" << shape.nrows << ", " << shape.ncols << ")";
            return os;
        }

        /// @brief Operator overload for getting rows and columns
        /// @param index 0 for rows, 1 for columns
        /// @return number of rows or columns
        int operator[](int index) const {
            if (index != 0 && index != 1) {
                throw std::runtime_error("Invalid data shape. Valid indices "
                                         "are 0 for rows and 1 for columns.");
            }
            return index == 0 ? nrows : ncols;
        }
    };

    /// @brief Class for Containing data in a row by column table format
    class DataTable {
    private:
        std::vector<std::string> headerOrder;
        std::map<std::string, std::vector<std::string>> data;
        DataTableShape shape;
        std::vector<int>
        convertStringVecToInt(std::vector<std::string> data) const;

        std::vector<std::string> loadRows(std::ifstream &csvStream);

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
        /// @brief Default constructor. Sets everything to default
        DataTable(){};

        /// @brief Constructor used to load a file to a DataTable
        /// @param filename file path as a string
        /// @param hasHeaders flag to read headers. False if no headers in file
        /// @param delim Signal character that separates the data
        DataTable(const std::string &filename, bool hasHeaders = true,
                  char delim = ',');

        /// @brief Constructor used to load an SQL Table to a DataTable
        /// @param dbfile file path to the database file
        /// @param tablename Table name to search for in the database
        DataTable(const std::string &dbfile, const std::string &tablename);

        /// @brief Constructor used to take a data map to a DataTable
        /// @param data map of string headers to vector columns
        /// @param shape class containing shape details
        /// @param headOrder order of which to write the headers
        DataTable(std::map<std::string, std::vector<std::string>> data,
                  DataTableShape shape,
                  std::vector<std::string> headOrder = {});

        /// @brief Default destructor
        ~DataTable(){};

        /// @brief Function used to load a DataTable to the provided CSV
        /// @param filename csv file to write data to
        void toCSV(const std::string &filename) const;

        /// @brief Function used to read from a file to the current DataTable
        /// @param filename csv file to read from
        /// @param hasHeaders flag to identify if there are headers
        /// @param delim Signal character that separates the data
        /// @return true if successful, false if failure
        bool fromCSV(const std::string &filename, bool hasHeaders = true,
                     char delim = ',');

        /// @brief Function used to read from a SQL Table to a DataTable
        /// @param dbfile database file containing the SQL Table to read from
        /// @param tablename SQL Table name to read
        /// @return true if successful, false if failure
        bool fromSQL(const std::string &dbfile, const std::string &tablename);

        /// @brief Function used to get the data row by row
        /// @return
        std::vector<std::vector<std::string>> getData() const {
            std::vector<std::vector<std::string>> dat;
            for (int i = 0; i < this->nrows(); ++i) {
                dat.push_back({});
            }
            for (auto kv : this->data) {
                for (int i = 0; i < kv.second.size(); ++i) {
                    dat[i].push_back(kv.second[i]);
                }
            }
            return dat;
        }

        /// @brief
        /// @param idx
        /// @return
        DataTable getRow(int idx) const;

        /// @brief
        /// @param columnName
        /// @return
        std::vector<std::string> getColumn(std::string columnName) const;

        /// @brief
        /// @param columnNames
        /// @return
        DataTable selectColumns(std::vector<std::string> columnNames) const;

        /// @brief
        /// @param idxs
        /// @return
        DataTable selectRows(std::vector<int> idxs) const;

        /// @brief
        /// @param start
        /// @param end
        /// @return
        DataTable selectRowRange(int start, int end) const;

        /// @brief
        /// @param columnDataMap
        /// @return
        DataTable selectWhere(
            std::unordered_map<std::string, std::string> columnDataMap) const;

        /// @brief
        /// @param tableTwo
        /// @param tableOneColumnName
        /// @param tableTwoColumnName
        /// @return
        DataTable innerJoin(DataTable const &tableTwo,
                            std::string tableOneColumnName,
                            std::string tableTwoColumnName) const;

        /// @brief
        /// @param tableTwo
        /// @param tableOneColumnNames
        /// @param tableTwoColumnNames
        /// @return
        DataTable innerJoin(DataTable const &tableTwo,
                            std::vector<std::string> tableOneColumnNames,
                            std::vector<std::string> tableTwoColumnNames) const;

        /// @brief
        /// @param tableTwo
        /// @param tableOneColumnName
        /// @param tableTwoColumnName
        /// @return
        DataTable leftJoin(DataTable const &tableTwo,
                           std::string tableOneColumnName,
                           std::string tableTwoColumnName) const;

        /// @brief
        /// @param tableTwo
        /// @param tableOneColumnName
        /// @param tableTwoColumnName
        /// @return
        DataTable rightJoin(DataTable const &tableTwo,
                            std::string tableOneColumnName,
                            std::string tableTwoColumnName) const;

        /// @brief
        /// @param tableTwo
        /// @param tableOneColumnName
        /// @param tableTwoColumnName
        /// @return
        DataTable outerJoin(DataTable const &tableTwo,
                            std::string tableOneColumnName,
                            std::string tableTwoColumnName) const;

        /// @brief
        /// @param n
        /// @return
        DataTable topNRows(int n) const { return this->selectRowRange(0, n); }

        /// @brief
        /// @param n
        /// @return
        DataTable bottomNRows(int n) const {
            return selectRowRange(shape[0] - n, shape[0]);
        }

        /// @brief
        /// @return
        DataTable head() const { return topNRows(10); }

        /// @brief
        /// @return
        DataTable tail() const { return bottomNRows(10); }

        /// @brief
        /// @return
        std::vector<std::string> getHeaders() const {
            return this->headerOrder;
        }

        // table operations

        /// @brief
        /// @param column_names
        void dropColumns(std::vector<std::string> column_names);

        /// @brief
        /// @param column
        void dropColumn(std::string column);

        /// @brief
        /// @param seed
        void shuffleRows(int seed = 0);

        /// @brief
        /// @param column
        /// @return
        std::string min(std::string column) const;

        /// @brief
        /// @param column
        /// @return
        std::string max(std::string column) const;

        /// @brief
        /// @return
        std::string min() const;

        /// @brief
        /// @return
        std::string max() const;

        /// @brief
        /// @param column
        /// @return
        double sum(std::string column) const;

        /// @brief
        /// @return
        double sum() const;

        /// @brief
        /// @param column
        /// @return
        double mean(std::string column) const;

        /// @brief
        /// @return
        double mean() const;

        // overridden operators

        /// @brief
        /// @param idx
        /// @return
        DataTable operator[](int idx) const; // select row

        /// @brief
        /// @param columnName
        /// @return
        std::vector<std::string>
        operator[](std::string columnName) const; // select column into DT

        /// @brief
        /// @param os
        /// @param table
        /// @return
        friend std::ostream &operator<<(std::ostream &os,
                                        const DataTable &table);

        /// @brief
        /// @param tableTwo
        /// @return
        DataTable operator+(DataTable const &tableTwo) const;

        // for other classes

        /// @brief
        /// @return
        int nrows() const { return shape[0]; }

        /// @brief
        /// @return
        int ncols() const { return shape[1]; }

        /// @brief
        /// @return
        DataTableShape getShape() const { return shape; }

        /// @brief
        /// @return
        bool empty() const { return this->data.empty(); }
    };
} // namespace Data

#endif