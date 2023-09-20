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

TEST_F(DataTableTest, ReadFromCSV) {
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