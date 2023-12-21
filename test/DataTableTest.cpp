#include "DataTable.hpp"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

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
        std::vector<std::string> expectedHeaders = {"0", "1", "2"};
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
        Data::DataTable data = dt.getRow(1);

        EXPECT_EQ(expectedData[0], data["Test"][0]);
        EXPECT_EQ(expectedData[1], data["Test1"][0]);
        EXPECT_EQ(expectedData[2], data["Test2"][0]);
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
    // try {
    std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};

    // Seeded so expect to be the same each time
    std::vector<std::vector<std::string>> expectedData = {
        {"1", "4", "7", "10"}, {"2", "5", "8", "11"}, {"3", "6", "9", "12"}};

    std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n10,11,12\n";
    outStream << csvData;
    outStream.close();

    Data::DataTable dt;
    std::string tFilePath = mTempFileAbsolute.string();
    dt.fromCSV(tFilePath, true);
    EXPECT_THROW(dt.shuffleRows(), std::logic_error);

    //     EXPECT_EQ(expectedHeaders, dt.getHeaders());
    //     EXPECT_EQ(expectedData, dt.getData());
    // } catch (const std::exception &e_) {
    //     FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
    //            << e_.what();
    // }
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
        std::string result = dt.min("Test1");
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
        std::string result = dt.max("Test1");
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
        double result = dt.sum("Test1");
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
        double result = dt.mean("Test1");
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
        Data::DataTable result = dt[1];
        EXPECT_EQ(expected[0], result["Test"][0]);
        EXPECT_EQ(expected[1], result["Test1"][0]);
        EXPECT_EQ(expected[2], result["Test2"][0]);
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

TEST_F(DataTableTest, selectWhere) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2", "3"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n4,5,6\n7,8,9\n";

        std::unordered_map<std::string, std::string> selectMap;
        selectMap["Test"] = "1";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.selectWhere(selectMap);

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectWhereMultiHit) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2", "3"},
                                                              {"1", "4", "6"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n1,4,6\n7,8,9\n";

        std::unordered_map<std::string, std::string> selectMap;
        selectMap["Test"] = "1";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.selectWhere(selectMap);

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, selectWhereMultiSelect) {
    try {
        std::vector<std::string> expectedHeaders = {"Test", "Test1", "Test2"};
        std::vector<std::vector<std::string>> expectedData = {{"1", "2", "3"}};
        std::string csvData = "Test,Test1,Test2\n1,2,3\n1,4,6\n7,8,9\n";

        std::unordered_map<std::string, std::string> selectMap;
        selectMap["Test"] = "1";
        selectMap["Test1"] = "2";
        outStream << csvData;
        outStream.close();

        Data::DataTable dt;
        std::string tFilePath = mTempFileAbsolute.string();
        dt.fromCSV(tFilePath, true);
        Data::DataTable resultDT = dt.selectWhere(selectMap);

        EXPECT_EQ(expectedHeaders, resultDT.getHeaders());
        EXPECT_EQ(expectedData, resultDT.getData());
    } catch (const std::exception &e_) {
        FAIL() << "Caught an exception in " << typeid(*this).name() << ": "
               << e_.what();
    }
}

TEST_F(DataTableTest, innerJoin) {
    std::vector<std::string> headerOrder1 = {"id", "test1", "test2", "test3"};
    std::map<std::string, std::vector<std::string>> d1;
    d1["id"] = {"1", "2", "2"};
    d1["test1"] = {"hi1.1", "hi2.1", "hi3.1"};
    d1["test2"] = {"hi1.2", "hi2.2", "hi3.2"};
    d1["test3"] = {"hi1.3", "hi2.3", "hi3.3"};

    Data::DataTableShape shape1;
    shape1.setNCols(4);
    shape1.setNRows(3);

    std::vector<std::string> headerOrder2 = {"id", "test4", "test5", "test6"};
    std::map<std::string, std::vector<std::string>> d2;
    d2["id"] = {"1", "2", "3"};
    d2["test4"] = {"hi1.4", "hi2.4", "hi3.4"};
    d2["test5"] = {"hi1.5", "hi2.5", "hi3.5"};
    d2["test6"] = {"hi1.6", "hi2.6", "hi3.6"};

    Data::DataTableShape shape2;
    shape2.setNCols(4);
    shape2.setNRows(3);

    Data::DataTable dt1(d1, shape1, headerOrder1);
    Data::DataTable dt2(d2, shape2, headerOrder2);

    Data::DataTable resultTable = dt1.innerJoin(dt2, "id", "id");
    std::vector<std::string> resultHeaders = resultTable.getHeaders();
    std::vector<std::vector<std::string>> resultData = resultTable.getData();

    std::vector<std::string> expectedHeaders = {
        "id", "test1", "test2", "test3", "test4", "test5", "test6"};

    ASSERT_TRUE(resultHeaders.size() == expectedHeaders.size());

    std::vector<std::vector<std::string>> expectedData = {
        {"1", "hi1.1", "hi1.2", "hi1.3", "hi1.4", "hi1.5", "hi1.6"},
        {"2", "hi2.1", "hi2.2", "hi2.3", "hi2.4", "hi2.5", "hi2.6"},
        {"2", "hi3.1", "hi3.2", "hi3.3", "hi2.4", "hi2.5", "hi2.6"}};

    ASSERT_TRUE(resultData.size() == expectedData.size());
    ASSERT_TRUE(resultData[0].size() == expectedData[0].size());

    for (int i = 0; i < resultHeaders.size(); ++i) {
        EXPECT_EQ(resultHeaders[i], expectedHeaders[i]);
    }

    for (int i = 0; i < resultData.size(); ++i) {
        for (int j = 0; j < resultData[0].size(); ++j) {
            EXPECT_EQ(resultData[i][j], expectedData[i][j]);
        }
    }
}

TEST_F(DataTableTest, multiInnerJoin) {
    std::map<std::string, std::vector<std::string>> d1;
    d1["id"] = {"1", "2", "2", "3"};
    d1["id2"] = {"a", "b", "c", "c"};
    d1["test1"] = {"hi1.1", "hi2.1", "hi3.1", "hi4.1"};
    d1["test2"] = {"hi1.2", "hi2.2", "hi3.2", "hi4.2"};
    d1["test3"] = {"hi1.3", "hi2.3", "hi3.3", "hi4.3"};

    Data::DataTableShape shape1;
    shape1.setNCols(5);
    shape1.setNRows(3);

    std::map<std::string, std::vector<std::string>> d2;
    d2["id"] = {"1", "2", "3"};
    d2["id2"] = {"a", "b", "b"};
    d2["test4"] = {"hi1.4", "hi2.4", "hi3.4"};
    d2["test5"] = {"hi1.5", "hi2.5", "hi3.5"};
    d2["test6"] = {"hi1.6", "hi2.6", "hi3.6"};

    Data::DataTableShape shape2;
    shape2.setNCols(5);
    shape2.setNRows(3);

    Data::DataTable dt1(d1, shape1);
    Data::DataTable dt2(d2, shape2);

    std::vector<std::string> columnNames = {"id", "id2"};
    Data::DataTable resultTable = dt1.innerJoin(dt2, columnNames, columnNames);
    std::vector<std::string> resultHeaders = resultTable.getHeaders();
    std::vector<std::vector<std::string>> resultData = resultTable.getData();

    std::vector<std::string> expectedHeaders = {
        "id", "id2", "test1", "test2", "test3", "test4", "test5", "test6"};

    std::vector<std::vector<std::string>> expectedData = {
        {"1", "a", "hi1.1", "hi1.2", "hi1.3", "hi1.4", "hi1.5", "hi1.6"},
        {"2", "b", "hi2.1", "hi2.2", "hi2.3", "hi2.4", "hi2.5", "hi2.6"}};

    ASSERT_TRUE(resultData.size() == expectedData.size());
    ASSERT_TRUE(resultData[0].size() == expectedData[0].size());

    for (int i = 0; i < resultHeaders.size(); ++i) {
        EXPECT_EQ(resultHeaders[i], expectedHeaders[i]);
    }

    for (int i = 0; i < resultData.size(); ++i) {
        for (int j = 0; j < resultData[0].size(); ++j) {
            EXPECT_EQ(resultData[i][j], expectedData[i][j]);
        }
    }
}

TEST_F(DataTableTest, operatorPlusOverload) {
    std::map<std::string, std::vector<std::string>> d1;
    d1["id"] = {"1", "2", "2"};
    d1["test1"] = {"hi1.1", "hi2.1", "hi3.1"};
    d1["test2"] = {"hi1.2", "hi2.2", "hi3.2"};
    d1["test3"] = {"hi1.3", "hi2.3", "hi3.3"};

    Data::DataTableShape shape1;
    shape1.setNCols(5);
    shape1.setNRows(3);

    std::map<std::string, std::vector<std::string>> d2;
    d2["id"] = {"1", "2", "3"};
    d2["test1"] = {"hi1.4", "hi2.4", "hi3.4"};
    d2["test2"] = {"hi1.5", "hi2.5", "hi3.5"};
    d2["test3"] = {"hi1.6", "hi2.6", "hi3.6"};

    Data::DataTableShape shape2;
    shape2.setNCols(5);
    shape2.setNRows(3);

    Data::DataTable dt1(d1, shape1);
    Data::DataTable dt2(d2, shape2);

    Data::DataTable resultTable = dt1 + dt2;
    std::vector<std::string> resultHeaders = resultTable.getHeaders();
    std::vector<std::vector<std::string>> resultData = resultTable.getData();

    std::vector<std::string> expectedHeaders = {"id", "test1", "test2",
                                                "test3"};

    std::vector<std::vector<std::string>> expectedData = {
        {"1", "hi1.1", "hi1.2", "hi1.3"}, {"2", "hi2.1", "hi2.2", "hi2.3"},
        {"2", "hi3.1", "hi3.2", "hi3.3"}, {"1", "hi1.4", "hi1.5", "hi1.6"},
        {"2", "hi2.4", "hi2.5", "hi2.6"}, {"3", "hi3.4", "hi3.5", "hi3.6"}};

    ASSERT_TRUE(resultData.size() == expectedData.size());
    ASSERT_TRUE(resultData[0].size() == expectedData[0].size());

    for (int i = 0; i < resultHeaders.size(); ++i) {
        EXPECT_EQ(resultHeaders[i], expectedHeaders[i]);
    }

    for (int i = 0; i < resultData.size(); ++i) {
        for (int j = 0; j < resultData[0].size(); ++j) {
            EXPECT_EQ(resultData[i][j], expectedData[i][j]);
        }
    }
}
