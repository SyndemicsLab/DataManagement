////////////////////////////////////////////////////////////////////////////////
// File: test_csv_source.cpp                                                  //
// Project: src                                                               //
// Created Date: Th Feb 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: Thu Feb 20 2025                                             //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
// -----                                                                      //
// HISTORY:                                                                   //
// Date      	By	Comments                                                  //
// ----------	---	--------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////

#include <datamanagement/source/csv_source.hpp>
#include <gtest/gtest.h>

class CSVSourceTest : public ::testing::Test {
    void SetUp() override {
        // Create a temporary CSV file for testing
        std::ofstream file("test.csv");
        file << "id,name,age\n";
        file << "1,Alice,30\n";
        file << "2,Bob,25\n";
        file << "3,Charlie,35\n";
        file.close();
    }

    void TearDown() override {
        // Remove the temporary CSV file after testing
        std::remove("test.csv");
    }
};

TEST_F(CSVSourceTest, GetData) {
    datamanagement::source::CSVSource csv_source;
    csv_source.ConnectToFile("test.csv");

    // Test without where conditions
    Eigen::MatrixXd data = csv_source.GetData({"id", "age"}, {});
    EXPECT_EQ(data.rows(), 3);
    EXPECT_EQ(data.cols(), 2);

    // Test with where conditions
    Eigen::MatrixXd filtered_data =
        csv_source.GetData({"id", "age"}, {{"name", "Alice"}});
    EXPECT_EQ(filtered_data.rows(), 1);
    EXPECT_EQ(filtered_data.cols(), 2);
}

TEST_F(CSVSourceTest, WriteCSV) {
    datamanagement::source::CSVSource csv_source1;
    csv_source1.ConnectToFile("test.csv");

    csv_source1.WriteCSV("output.csv", {"id", "age"});
    datamanagement::source::CSVSource csv_source2;
    csv_source2.ConnectToFile("output.csv");

    // Test without where conditions
    Eigen::MatrixXd data1 = csv_source1.GetData({"id", "age"}, {});
    Eigen::MatrixXd data2 = csv_source2.GetData({"id", "age"}, {});
    ASSERT_TRUE(data2.isApprox(data1));
    std::remove("output.csv");
}