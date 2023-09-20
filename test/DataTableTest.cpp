#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "DataTable.hpp"

class DataTableTest : public ::testing::Test {
protected:
    boost::filesystem::path mTempFileRelative;
    boost::filesystem::path mTempFileAbsolute;
    std::ofstream outStream;

    void SetUp() override {
        mTempFileRelative =
            boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.csv");
        mTempFileAbsolute =
            boost::filesystem::temp_directory_path() / mTempFileRelative;
        outStream.open(mTempFileAbsolute);
    }

    void TearDown() override {
        if (outStream.is_open()) {
            outStream.close();
        }
    }
};

TEST_F(DataTableTest, ReadFromCSVHeader) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        bool res = dt.fromCSV(tFilePath, true);
        std::vector<std::string> headers = dt.getHeaders();

        EXPECT_TRUE(res);
        EXPECT_EQ(expectedHeaders, headers);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, ReadFromCSVNoHeader) {
    try {
        std::vector<std::string> expectedHeaders = {};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2", "3"}};
        std::string csvData = "1,2,3\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        bool res = dt.fromCSV(tFilePath, false);
        std::vector<std::string> headers = dt.getHeaders();
        std::vector<std::vector<std::string>> data = dt.getData();

        EXPECT_TRUE(res);
        EXPECT_EQ(expectedHeaders, headers);
        EXPECT_EQ(expectedData, data);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, ReadFromCSVHeaderDelimiter) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2", "3"}};
        std::string csvData = "Test;Test1;Test2\n1;2;3\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        bool res = dt.fromCSV(tFilePath, true, ';');
        std::vector<std::string> headers = dt.getHeaders();
        std::vector<std::vector<std::string>> data = dt.getData();

        EXPECT_TRUE(res);
        EXPECT_EQ(expectedHeaders, headers);
        EXPECT_EQ(expectedData, data);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, GetRow) {
    try {
        std::vector<std::string> expectedData = {"4", "5", "6"};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<std::string> data = dt.getRow(1);

        EXPECT_EQ(expectedData, data);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, GetColumnInt) {
    try {
        std::vector<std::string> expectedData = {"1", "4"};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<std::string> data = dt.getColumn(0);

        EXPECT_EQ(expectedData, data);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, GetColumnString) {
    try {
        std::vector<std::string> expectedData = {"3", "6"};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<std::string> data = dt.getColumn("Test2");

        EXPECT_EQ(expectedData, data);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectColumnsInt) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2"},
                                                              {"4", "5"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.selectColumns({0, 1});

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectColumnsStrings) {
    try {
        std::vector<std::string> expectedHeaders = {"Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"2", "3"},
                                                              {"5", "6"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<std::string> columnNames = {"Test1", "Test2"};
        Data::DataTable resultDT = dt.selectColumns(columnNames);

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectRowsInt) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2", "3"},
                                                              {"7", "8", "9"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.selectRows({0, 2});

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectRowRange) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"4", "5", "6"},
                                                              {"7", "8", "9"}};
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.selectRowRange(1, 3);

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectTopNRows) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2", "3"},
                                                              {"4", "5", "6"}};
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.topNRows(2);

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectBottomNRows) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {
            {"7", "8", "9"}, {"10", "11", "12"}};
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.bottomNRows(2);

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, head) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {
            {"1", "2", "3"},
            {"4", "5", "6"},
            {"7", "8", "9"},
            {"10", "11", "12"}};
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.head();

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, tail) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {
            {"1", "2", "3"},
            {"4", "5", "6"},
            {"7", "8", "9"},
            {"10", "11", "12"}};
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.tail();

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, dropColumnInt) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2"},
                                                              {"4", "5"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        dt.dropColumn(2);

        EXPECT_EQ(expectedHeaders, dt.getHeaders());
        EXPECT_EQ(expectedData, dt.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, dropColumnString) {
    try {
        std::vector<std::string> expectedHeaders = {"Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"2", "3"},
                                                              {"5", "6"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        dt.dropColumn("Test");

        EXPECT_EQ(expectedHeaders, dt.getHeaders());
        EXPECT_EQ(expectedData, dt.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, dropColumnsInt) {
    try {
        std::vector<std::string> expectedHeaders = {"Test"};
        std::vector<std::vector<std::string>> expectedData = {{"1"}, {"4"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<int> columnsDropped = {1, 2};
        dt.dropColumns(columnsDropped);

        EXPECT_EQ(expectedHeaders, dt.getHeaders());
        EXPECT_EQ(expectedData, dt.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, dropColumnsString) {
    try {
        std::vector<std::string> expectedHeaders = {"Test1"};
        std::vector<std::vector<std::string>> expectedData = {{"2"}, {"5"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<std::string> columnsDropped = {"Test", "Test2"};
        dt.dropColumns(columnsDropped);

        EXPECT_EQ(expectedHeaders, dt.getHeaders());
        EXPECT_EQ(expectedData, dt.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, shuffle) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};

        // Seeded so expect to be the same each time
        std::vector<std::vector<std::string>> expectedData = {
            {"7", "8", "9"},
            {"10", "11", "12"},
            {"4", "5", "6"},
            {"1", "2", "3"}};

        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        dt.shuffleRows();

        EXPECT_EQ(expectedHeaders, dt.getHeaders());
        EXPECT_EQ(expectedData, dt.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, minIdx) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::string result = dt.min(1);
        EXPECT_EQ("2", result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, minOverall) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::string result = dt.min();
        EXPECT_EQ("1", result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, maxIdx) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::string result = dt.max(1);
        EXPECT_EQ("11", result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, maxOverall) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::string result = dt.max();
        EXPECT_EQ("12", result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, sumIdx) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        double result = dt.sum(1);
        EXPECT_EQ(26.0, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, sumOverall) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        double result = dt.sum();
        EXPECT_EQ(78.0, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, meanIdx) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        double result = dt.mean(1);
        EXPECT_EQ(26.0 / 4.0, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, meanOverall) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        double result = dt.mean();
        EXPECT_EQ(78.0 / 12.0, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, operationalRowIndexing) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();
        std::vector<std::string> expected = {"4", "5", "6"};

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<std::string> result = dt[1];
        EXPECT_EQ(expected, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, operationalColumnIndexing) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();
        std::vector<std::string> expected = {"2", "5", "8", "11"};

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        std::vector<std::string> result = dt["Test1"];
        EXPECT_EQ(expected, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, nrows) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        int result = dt.nrows();
        EXPECT_EQ(4, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, ncols) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        int result = dt.ncols();
        EXPECT_EQ(3, result);
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, getShape) {
    try {
        std::string csvData =
            "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
        outStream << csvData;
        outStream.close();
        Data::DataTableShape expected;
        expected.setNCols(3);
        expected.setNRows(4);

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTableShape result = dt.getShape();
        EXPECT_EQ(expected.getNRows(), result.getNRows());
        EXPECT_EQ(expected.getNCols(), result.getNCols());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}
