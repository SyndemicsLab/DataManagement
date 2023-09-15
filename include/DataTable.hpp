#ifndef DATATABLE_HPP_
#define DATATABLE_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Built in part on the back of:
// https://github.com/anthonymorast/DataTables/tree/master

namespace Data {
    struct DataShapeException : public std::runtime_error {
        DataShapeException(const std::string msg) : runtime_error(msg){};
        ~DataShapeException(){};
    };

    struct DataTableShape {
        int nrows;
        int ncols;
        friend std::ostream &operator<<(std::ostream &os,
                                        const DataTableShape shape) {
            os << "(" << shape.nrows << ", " << shape.ncols << ")";
            return os;
        }

        int operator[](int index) const {
            if (index != 0 && index != 1) {
                throw DataShapeException("Invalid data shape. Valid indices "
                                         "are 0 for rows and 1 for columns.");
            }
            return index == 0 ? nrows : ncols;
        }
    };

    class DataTable {
    private:
        std::vector<std::string> _headers;
        std::shared_ptr<std::shared_ptr<std::string>> _data;
        DataTableShape _shape;

    public:
        DataTable(){};
        ~DataTable(){};
        void to_csv(const std::string &filename) const;
        bool from_csv(const std::string &filename, bool has_headers = true);
        bool from_sql(const std::string &dbfile, const std::string &tablename);
        std::shared_ptr<std::shared_ptr<std::string>> get_data() const {
            return _data;
        }
        std::shared_ptr<std::string> get_row(int idx) const;
        std::shared_ptr<std::string> get_column(int idx) const;
        std::shared_ptr<std::string> get_column(std::string column_name) const;
        DataTable select_columns(std::vector<int> column_idxs) const;
        DataTable select_columns(std::vector<std::string> column_names) const;
        DataTable select_rows(std::vector<int> idxs) const;
        DataTable select_row_range(int start, int end) const;
        DataTable top_n_rows(int n) const {
            return this->select_row_range(0, n);
        }
        DataTable bottom_n_rows(int n) const {
            return select_row_range(_shape[0] - n, _shape[0]);
        }
        DataTable head() const { return top_n_rows(10); }
        DataTable tail() const { return bottom_n_rows(10); }
        std::vector<std::string> get_headers() const { return _headers; }

        // table operations
        void drop_columns(std::vector<int> columns);
        void drop_columns(std::vector<std::string> column_names);
        void drop_column(int column);
        void drop_column(std::string column);
        void shuffle_rows(int passes = 100);

        // data analysis
        std::shared_ptr<std::string> pct_change(std::string column) const;
        std::shared_ptr<std::string> pct_change(int column) const;
        std::shared_ptr<std::string> sma(std::string column, int periods) const;
        std::shared_ptr<std::string> sma(int column, int periods) const;
        std::shared_ptr<std::string> ema(std::string column, int periods) const;
        std::shared_ptr<std::string> ema(int column, int periods) const;
        std::shared_ptr<std::string> rsi(std::string column,
                                         int periods = 14) const;
        std::shared_ptr<std::string> rsi(int column, int periods = 14) const;
        std::string min(int col) const;
        std::string max(int col) const;
        std::string min() const;
        std::string max() const;
        double sum(int col) const;
        double sum() const;
        double mean(int col) const;
        double mean() const;

        // overridden operators
        std::shared_ptr<std::string> operator[](int index) const; // select row
        DataTable operator[](std::string column) const; // select column into DT
        template <typename U>
        friend std::ostream &operator<<(std::ostream &os,
                                        const DataTable &table);

        // for other classes
        int nrows() const { return _shape[0]; }
        int ncols() const { return _shape[1]; }
        DataTableShape shape() const { return _shape; }
    };
} // namespace Data

#endif