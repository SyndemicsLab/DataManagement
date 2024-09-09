#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "DataManager.hpp"

class DataManagerTest : public ::testing::Test {
protected:
    std::filesystem::path tempPath;
    std::filesystem::path absolute;
    std::ofstream tempStream;
    datamanagement::DataManager testManager;
    std::string testTableName;
    void SetUp() override {
        tempPath = std::tmpnam(nullptr) + std::string(".csv");
        absolute = std::filesystem::temp_directory_path() / tempPath;
        tempStream.open(absolute);
        std::filesystem::path t = std::tmpnam(nullptr) + std::string(".csv");
        std::filesystem::path a = std::filesystem::temp_directory_path() / t;
        testTableName = a.filename().stem();
        std::ofstream s;
        s.open(a);
        s << "block,agegrp,sex,oud,counts" << std::endl
          << "No_Treatment,10_14,Male,Active_Noninjection,2917.55795376043"
          << std::endl
          << "No_Treatment,10_14,Male,Active_Injection,977.390032367151"
          << std::endl
          << "No_Treatment,10_14,Male,Nonactive_Noninjection,288.995723856067";
        s.close();
        int rc = testManager.AddCSVTable(a);
    }
    void TearDown() override {
        if (tempStream.is_open()) {
            tempStream.close();
        }
    }
};

TEST_F(DataManagerTest, Constructor) {
    datamanagement::DataManager manager;
    ASSERT_TRUE(true); // ensure we create the object
}

TEST_F(DataManagerTest, AddCSVTable) {
    tempStream
        << "block,agegrp,sex,oud,counts" << std::endl
        << "No_Treatment,10_14,Male,Active_Noninjection,2917.55795376043"
        << std::endl
        << "No_Treatment,10_14,Male,Active_Injection,977.390032367151"
        << std::endl
        << "No_Treatment,10_14,Male,Nonactive_Noninjection,288.995723856067";
    tempStream.close();

    int rc = testManager.AddCSVTable(absolute);
    datamanagement::Table data = {};
    rc = testManager.Select(
        "select count(*) from sqlite_master as tables where type='table'",
        data);
    ASSERT_EQ(rc, 0);
    ASSERT_EQ(std::stoi(data[0][0]), 2);
}

TEST_F(DataManagerTest, Create) {
    std::string sql = "CREATE TABLE WORDS("
                      "ID INT PRIMARY KEY  NOT NULL,"
                      "CURRENT_WORD   TEXT NOT NULL,"
                      "BEFORE_WORD    TEXT NOT NULL,"
                      "AFTER_WORD     TEXT NOT NULL,"
                      "OCCURANCES     INT  NOT NULL);";
    datamanagement::Table data = {};
    testManager.Create(sql, data);
    // verify a correct value returns (SQLITE_OK == 0)
    ASSERT_EQ(testManager.Select("SELECT * from WORDS", data), 0);
}

TEST_F(DataManagerTest, Select) {
    datamanagement::Table data = {};
    std::string sql = "SELECT * FROM '" + testTableName + "';";
    testManager.Select(sql, data);
    std::vector<std::string> expected = {"No_Treatment",
                                         "10_14",
                                         "Male",
                                         "Active_Noninjection",
                                         "2917.55795376043",
                                         "No_Treatment",
                                         "10_14",
                                         "Male",
                                         "Active_Injection",
                                         "977.390032367151",
                                         "No_Treatment",
                                         "10_14",
                                         "Male",
                                         "Nonactive_Noninjection",
                                         "288.995723856067"};
    int i = 0;
    for (datamanagement::Row row : data) {
        for (std::string d : row) {
            ASSERT_EQ(expected[i], d);
            ++i;
        }
    }
}

TEST_F(DataManagerTest, Update) {
    std::string sql = "CREATE TABLE WORDS("
                      "ID INT PRIMARY KEY  NOT NULL,"
                      "CURRENT_WORD   TEXT NOT NULL,"
                      "BEFORE_WORD    TEXT NOT NULL,"
                      "AFTER_WORD     TEXT NOT NULL,"
                      "OCCURANCES     INT  NOT NULL);";
    datamanagement::Table data = {};
    testManager.Create(sql, data);
    sql.clear();
    sql = "INSERT INTO WORDS VALUES (1, 'test', 'ah', 'ha', 10)";
    data.clear();
    int rc = testManager.Update(sql, data);
    ASSERT_EQ(rc, 0);
}

TEST_F(DataManagerTest, Delete) {
    std::string sql = "CREATE TABLE WORDS("
                      "ID INT PRIMARY KEY  NOT NULL,"
                      "CURRENT_WORD   TEXT NOT NULL,"
                      "BEFORE_WORD    TEXT NOT NULL,"
                      "AFTER_WORD     TEXT NOT NULL,"
                      "OCCURANCES     INT  NOT NULL);";
    datamanagement::Table data = {};
    testManager.Create(sql, data);
    sql.clear();
    sql = "INSERT INTO WORDS VALUES (1, 'test', 'ah', 'ha', 10);";
    data.clear();
    testManager.Update(sql, data);
    data.clear();
    testManager.Delete("DELETE FROM WORDS WHERE ID = 1;", data);
    data.clear();
    testManager.Select("SELECT * FROM WORDS;", data);
    ASSERT_TRUE(data.empty());
}
