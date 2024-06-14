//===-- DataTable.hpp --------------------------------------------*- C++-*-===//
//
// Part of the Syndemics Lab DataManagement Code Repository, under
// the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of the DataTableShape and DataTable
/// interface and base class. In addition to the class definitions, a typedef
/// is provded to utilize a pointer of the DataTable object.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef DATATABLE_HPP_
#define DATATABLE_HPP_

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

/// @brief Namespace defining Data Containers
namespace Data {

    /// @brief Interface describing DataTableShape
    class IDataTableShape {
    public:
        /// @brief Default Destructor
        virtual ~IDataTableShape() = default;

        /// @brief Getter for the Number of Rows
        /// @return Number of Rows
        virtual int getNRows() const = 0;

        /// @brief Getter for the Number of Columns
        /// @return Number of Columns
        virtual int getNCols() const = 0;

        /// @brief Setter for Number of Rows
        /// @param nrows Number of Rows
        virtual void setNRows(int nrows) = 0;

        /// @brief Setter for Number of Columns
        /// @param ncols Number of Columns
        virtual void setNCols(int ncols) = 0;

        /// @brief Operator Overload to get number of columns or rows
        /// @param index 0 for Row, 1 for Column
        /// @return Number of Rows or Columns depending on parameter
        virtual int operator[](int index) const = 0;
    };

    /// @brief Helper Class used to hold DataTable Shapes
    class DataTableShape : public IDataTableShape {
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
        int getNRows() const override { return this->nrows; }

        /// @brief Getter for Number of Columns
        /// @return Number of Columns as an Integer
        int getNCols() const override { return this->ncols; }

        /// @brief Setter for Number of Rows
        /// @param nrows Number of Rows
        void setNRows(int nrows) override {
            this->nrows = (nrows < 0) ? 0 : nrows;
        }

        /// @brief Setter for Number of Columns
        /// @param ncols Number of Columns
        void setNCols(int ncols) override {
            this->ncols = (ncols < 0) ? 0 : ncols;
        }

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
        int operator[](int index) const override;
    };

    class IDataTable {
    public:
        virtual ~IDataTable() = default;
        virtual void toCSV(const std::string &filename) const = 0;
        virtual bool fromCSV(const std::string &filename,
                             bool hasHeaders = true, char delim = ',') = 0;
        virtual bool fromSQL(const std::string &dbfile,
                             const std::string &tablename) = 0;
        virtual std::vector<std::vector<std::string>> getData() const = 0;
        virtual std::map<std::string, std::vector<std::string>>
        getDataAsMap() const = 0;
        virtual std::shared_ptr<IDataTable> getRow(int idx) const = 0;
        virtual std::vector<std::string>
        getColumn(std::string columnName) const = 0;
        virtual bool checkColumnExists(std::string columnName) const = 0;
        virtual std::vector<std::string> getColumnNames() const = 0;
        virtual std::shared_ptr<IDataTable>
        selectColumns(std::vector<std::string> columnNames) const = 0;
        virtual std::shared_ptr<IDataTable>
        selectRows(std::vector<int> idxs) const = 0;
        virtual std::shared_ptr<IDataTable> selectRowRange(int start,
                                                           int end) const = 0;
        virtual std::shared_ptr<IDataTable>
        selectWhere(std::unordered_map<std::string, std::string> columnDataMap)
            const = 0;
        virtual std::shared_ptr<IDataTable>
        innerJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::string tableOneColumnName,
                  std::string tableTwoColumnName) const = 0;
        virtual std::shared_ptr<IDataTable>
        innerJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::vector<std::string> tableOneColumnNames,
                  std::vector<std::string> tableTwoColumnNames) const = 0;

        virtual std::shared_ptr<IDataTable>
        leftJoin(std::shared_ptr<IDataTable> const tableTwo,
                 std::string tableOneColumnName,
                 std::string tableTwoColumnName) const = 0;

        virtual std::shared_ptr<IDataTable>
        rightJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::string tableOneColumnName,
                  std::string tableTwoColumnName) const = 0;

        virtual std::shared_ptr<IDataTable>
        outerJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::string tableOneColumnName,
                  std::string tableTwoColumnName) const = 0;

        virtual std::shared_ptr<IDataTable> topNRows(int n) const = 0;

        virtual std::shared_ptr<IDataTable> bottomNRows(int n) const = 0;

        virtual std::shared_ptr<IDataTable> head() const = 0;

        virtual std::shared_ptr<IDataTable> tail() const = 0;

        virtual std::vector<std::string> getHeaders() const = 0;

        virtual void dropColumns(std::vector<std::string> column_names) = 0;

        virtual void dropColumn(std::string column) = 0;

        virtual void shuffleRows(int seed = 0) = 0;

        virtual std::string min(std::string column) const = 0;

        virtual std::string max(std::string column) const = 0;

        virtual std::string min() const = 0;

        virtual std::string max() const = 0;

        virtual double sum(std::string column) const = 0;

        virtual double sum() const = 0;

        virtual double mean(std::string column) const = 0;

        virtual double mean() const = 0;

        virtual std::shared_ptr<IDataTable> operator[](int idx) const = 0;

        virtual std::vector<std::string>
        operator[](std::string columnName) const = 0;

        virtual std::shared_ptr<IDataTable>
        concat(IDataTable const &tableTwo) const = 0;

        virtual std::shared_ptr<IDataTable>
        operator+(IDataTable const &tableTwo) const = 0;

        virtual int nrows() const = 0;

        virtual int ncols() const = 0;

        virtual DataTableShape getShape() const = 0;

        virtual bool empty() const = 0;

        virtual bool isNull() const = 0;
    };

    using IDataTablePtr = std::shared_ptr<Data::IDataTable>;

    /// @brief Class for Containing data in a row by column table format
    class DataTable : public IDataTable {
    private:
        bool loaded = false;
        std::string filename = "";
        std::vector<std::string> headerOrder;
        std::map<std::string, std::vector<std::string>> data;
        DataTableShape shape;
        std::vector<int>
        convertStringVecToInt(std::vector<std::string> data) const;

        void columnErrorCheck(std::string const columnName) const {
            if (this->data.find(columnName) == this->data.end()) {
                std::string message = "Invalid Column Name of " + columnName +
                                      " for DataTable. Columns are: ";
                for (auto kv : this->data) {
                    message += (kv.first + " ");
                }
                throw std::logic_error(message);
            }
        }

        void rowErrorCheck(int const idx) const {
            if (idx > this->nrows()) {
                std::string message = "Invalid Row Index of " +
                                      std::to_string(idx) +
                                      " for DataTable with " +
                                      std::to_string(this->nrows()) + " rows.";
                throw std::logic_error(message);
            }
        }

        std::vector<std::string> loadRows(std::ifstream &csvStream);

        std::string loadRow(unsigned int lineNum) const;

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

        DataTable(const DataTable &dt);

        /// @brief Constructor used to load a file to a DataTable
        /// @param filename file path as a string
        /// @param hasHeaders flag to read headers. False if no headers in file
        /// @param delim Signal character that separates the data
        DataTable(const std::string &filename, bool hasHeaders = true,
                  char delim = ',', bool lazyLoad = false);

        DataTable(const std::string &filename, bool lazyLoad = false)
            : DataTable(filename, false, ',', lazyLoad) {}

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
        void toCSV(const std::string &filename) const override;

        /// @brief Function used to read from a file to the current DataTable
        /// @param filename csv file to read from
        /// @param hasHeaders flag to identify if there are headers
        /// @param delim Signal character that separates the data
        /// @return true if successful, false if failure
        bool fromCSV(const std::string &filename, bool hasHeaders = true,
                     char delim = ',') override;

        /// @brief Function used to read from a SQL Table to a DataTable
        /// @param dbfile database file containing the SQL Table to read from
        /// @param tablename SQL Table name to read
        /// @return true if successful, false if failure
        bool fromSQL(const std::string &dbfile,
                     const std::string &tablename) override;

        /// @brief Function used to get the data row by row
        /// @return vector of rows
        std::vector<std::vector<std::string>> getData() const override {
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

        std::map<std::string, std::vector<std::string>>
        getDataAsMap() const override {
            return this->data;
        }

        /// @brief Return a row of data
        /// @param idx The index of the row of data looked for
        /// @return std::shared_ptr<IDataTable> object with the single row of
        /// data
        std::shared_ptr<IDataTable> getRow(int idx) const override;

        /// @brief Return a column of data
        /// @param columnName The column name to return
        /// @return vector representing the column of data
        std::vector<std::string>
        getColumn(std::string columnName) const override;

        bool checkColumnExists(std::string columnName) const override;

        std::vector<std::string> getColumnNames() const override;

        /// @brief Return a set of columns of data as a new
        /// std::shared_ptr<IDataTable>
        /// @param columnNames the vector of column names to search for
        /// @return std::shared_ptr<IDataTable> containing a copy of the columns
        std::shared_ptr<IDataTable>
        selectColumns(std::vector<std::string> columnNames) const override;

        /// @brief Return a std::shared_ptr<IDataTable> containing the asked for
        /// rows
        /// @param idxs indices of the rows to return
        /// @return std::shared_ptr<IDataTable> object containing a copy of the
        /// rows
        std::shared_ptr<IDataTable>
        selectRows(std::vector<int> idxs) const override;

        /// @brief Return a std::shared_ptr<IDataTable> containing the asked for
        /// rows
        /// @param start start index of the rows asked for
        /// @param end ending index of the rows asked for
        /// @return std::shared_ptr<IDataTable> object containing a copy of the
        /// rows
        std::shared_ptr<IDataTable> selectRowRange(int start,
                                                   int end) const override;

        /// @brief Return a std::shared_ptr<IDataTable> containing the data
        /// based on a where
        /// @param columnDataMap Map of column -> value map to search for in the
        /// std::shared_ptr<IDataTable>
        /// @return std::shared_ptr<IDataTable> containing a copy of the values
        /// searched for
        std::shared_ptr<IDataTable>
        selectWhere(std::unordered_map<std::string, std::string> columnDataMap)
            const override;

        /// @brief Inner Join done between the current
        /// std::shared_ptr<IDataTable> and the provided parameter
        /// @param tableTwo The other table to join with
        /// @param tableOneColumnName std::shared_ptr<IDataTable> column name to
        /// join on
        /// @param tableTwoColumnName std::shared_ptr<IDataTable> column name to
        /// join on
        /// @return Copy of the joined DataTables
        std::shared_ptr<IDataTable>
        innerJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::string tableOneColumnName,
                  std::string tableTwoColumnName) const override;

        /// @brief Inner Join done between the current IDataTable and the
        /// provided parameter
        /// @param tableTwo The other table to join with
        /// @param tableOneColumnNames IDataTable column names to join on
        /// @param tableTwoColumnNames IDataTable column names to join on
        /// @return Copy of the joined DataTables
        std::shared_ptr<IDataTable>
        innerJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::vector<std::string> tableOneColumnNames,
                  std::vector<std::string> tableTwoColumnNames) const override;

        /// @brief Not implmented yet
        /// @param tableTwo
        /// @param tableOneColumnName
        /// @param tableTwoColumnName
        /// @return
        std::shared_ptr<IDataTable>
        leftJoin(std::shared_ptr<IDataTable> const tableTwo,
                 std::string tableOneColumnName,
                 std::string tableTwoColumnName) const override;

        /// @brief Not implmented yet
        /// @param tableTwo
        /// @param tableOneColumnName
        /// @param tableTwoColumnName
        /// @return
        std::shared_ptr<IDataTable>
        rightJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::string tableOneColumnName,
                  std::string tableTwoColumnName) const override;

        /// @brief Not implmented yet
        /// @param tableTwo
        /// @param tableOneColumnName
        /// @param tableTwoColumnName
        /// @return
        std::shared_ptr<IDataTable>
        outerJoin(std::shared_ptr<IDataTable> const tableTwo,
                  std::string tableOneColumnName,
                  std::string tableTwoColumnName) const override;

        /// @brief Return the top N rows
        /// @param n the number of rows to return from the top
        /// @return Copy of the top N Rows
        std::shared_ptr<IDataTable> topNRows(int n) const override {
            return this->selectRowRange(0, n);
        }

        /// @brief Return the bottom N Rows
        /// @param n the number of rows to return from the bottom
        /// @return Copy of the bottom N Rows
        std::shared_ptr<IDataTable> bottomNRows(int n) const override {
            return selectRowRange(shape[0] - n, shape[0]);
        }

        /// @brief Return a copy of the top 10 rows
        /// @return A Copy of the top 10 rows
        std::shared_ptr<IDataTable> head() const override {
            int val = (this->nrows() < 10) ? this->nrows() : 10;
            return topNRows(val);
        }

        /// @brief Return a copy of the bottom 10 rows
        /// @return A copy of the bottom 10 rows
        std::shared_ptr<IDataTable> tail() const override {
            int val = (this->nrows() < 10) ? this->nrows() : 10;
            return bottomNRows(val);
        }

        /// @brief Return a copy of the headers
        /// @return A copy of the headers
        std::vector<std::string> getHeaders() const override {
            return this->headerOrder;
        }

        // table operations

        /// @brief Drop the provided columns from the IDataTable
        /// @param column_names Vector of Columns to drop from the data table
        void dropColumns(std::vector<std::string> column_names) override;

        /// @brief Drop the provided column from the IDataTable
        /// @param column Column to drop from the data table
        void dropColumn(std::string column) override;

        /// @brief Not Implemented Yet
        /// @param seed
        void shuffleRows(int seed = 0) override;

        /// @brief Retrieve the Minimum value in a column
        /// @param column Column name to search
        /// @return The minimum value in the column
        std::string min(std::string column) const override;

        /// @brief Retrieve the Maximum value in a column
        /// @param column Column name to search
        /// @return The maximum value in the column
        std::string max(std::string column) const override;

        /// @brief Retrieve the Minimum value in the IDataTable
        /// @return The Minimum value in the IDataTable
        std::string min() const override;

        /// @brief Retrieve teh Maxmimum value in the IDataTable
        /// @return The Maximum value in the IDataTable
        std::string max() const override;

        /// @brief Retrieve the Sum of the Column
        /// @param column The column name to search
        /// @return The sum of the values in the Column
        double sum(std::string column) const override;

        /// @brief Retrieve the Sum of the IDataTable
        /// @return The sum of the values in the IDataTable
        double sum() const override;

        /// @brief Retrieve the Mean of the Column
        /// @param column The column name to search
        /// @return The mean of the values in the Column
        double mean(std::string column) const override;

        /// @brief Retrieve the Mean of the IDataTable
        /// @return The Mean of the IDataTable
        double mean() const override;

        // overridden operators

        /// @brief Row indexing
        /// @param idx Row to copy back
        /// @return Copy of the row as a IDataTable
        std::shared_ptr<IDataTable>
        operator[](int idx) const override; // select row

        /// @brief Column Indexing
        /// @param columnName Column to copy back
        /// @return copy of the column as a vector
        std::vector<std::string> operator[](
            std::string columnName) const override; // select column into DT

        /// @brief Not Implemented Yet
        /// @param os
        /// @param table
        /// @return
        friend std::ostream &
        operator<<(std::ostream &os, std::shared_ptr<IDataTable> const table);

        std::shared_ptr<IDataTable>
        concat(IDataTable const &tableTwo) const override;

        /// @brief Concatenate two tables
        /// @param tableTwo table to concatenate with
        /// @return Copy of concatenated DataTables
        std::shared_ptr<IDataTable>
        operator+(IDataTable const &tableTwo) const override {
            return concat(tableTwo);
        };

        // for other classes

        /// @brief Helper to return the number of rows
        /// @return Number of Rows
        int nrows() const override { return shape[0]; }

        /// @brief Helper to return the number of columns
        /// @return Number of Columns
        int ncols() const override { return shape[1]; }

        /// @brief Return the shape
        /// @return Shape
        DataTableShape getShape() const override { return shape; }

        /// @brief Test if the table is empty
        /// @return true if no rows, false if contains data
        bool empty() const override { return (this->nrows() == 0); }

        /// @brief Test if the table contains no data at all
        /// @return true if `this->data` is empty, false otherwise
        bool isNull() const override { return this->data.empty(); }
    };
} // namespace Data

#endif
