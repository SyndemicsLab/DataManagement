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
    std::filesystem::path configFile;
    std::ofstream tempStream;
    std::ofstream configStream;
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

        configFile = std::filesystem::temp_directory_path() /
                     std::filesystem::path("sim.conf");
        configStream.open(configFile);

        configStream << "[simulation]" << std::endl
                     << "duration = 52" << std::endl
                     << "aging_interval = 260" << std::endl
                     << "intervention_change_times = 52" << std::endl
                     << "entering_sample_change_times = 52" << std::endl
                     << "overdose_change_times = 52" << std::endl
                     << "stratified_entering_cohort = false" << std::endl
                     << std::endl
                     << "[state]" << std::endl
                     << "interventions = No_Treatment, Buprenorphine,"
                        "Naltrexone, Methadone, Detox, Post-Buprenorphine,"
                        "Post-Naltrexone, Post-Methadone, Post-Detox"
                        ""
                     << std::endl
                     << "ouds = Active_Noninjection, Active_Injection,"
                        "Nonactive_Noninjection, Nonactive_Injection"
                     << std::endl
                     << std::endl
                     << "[demographic]" << std::endl
                     << "age_groups = 10_14, 15_19, 20_24, 25_29, 30_34, "
                        "35_39, 40_44, 45_49, 50_54, 55_59, 60_64, 65_69, "
                        "70_74, 75_79, 80_84, 85_89, 90_94, 95_99 "
                     << std::endl
                     << "sex = Male, Female " << std::endl
                     << std::endl
                     << "[cost]" << std::endl
                     << "cost_analysis = true" << std::endl
                     << "cost_perspectives = healthcare" << std::endl
                     << "discount_rate = 0.0025 " << std::endl
                     << "reporting_interval = 1" << std::endl
                     << "cost_utility_output_timesteps = 52 " << std::endl
                     << "cost_category_outputs = false " << std::endl
                     << std::endl
                     << "[output] " << std::endl
                     << "per_intervention_predictions = true " << std::endl
                     << "general_outputs = false " << std::endl
                     << "general_stats_output_timesteps = 52";
        configStream.close();
    }
    void TearDown() override {
        if (tempStream.is_open()) {
            tempStream.close();
        }
        if (configStream.is_open()) {
            configStream.close();
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

TEST_F(DataManagerTest, LoadConfigAndGetFromConfig) {
    testManager.LoadConfig(configFile);
    std::string data = "";
    int rc = testManager.GetFromConfig("simulation.duration", data);
    ASSERT_EQ(data, "52");
}

TEST_F(DataManagerTest, GetConfigSectionCategories) {
    testManager.LoadConfig(configFile);
    std::vector<std::string> data = {};
    int rc = testManager.GetConfigSectionCategories("simulation", data);
    std::vector<std::string> expected = {"duration",
                                         "aging_interval",
                                         "intervention_change_times",
                                         "entering_sample_change_times",
                                         "overdose_change_times",
                                         "stratified_entering_cohort"};
    int i = 0;
    for (std::string d : data) {
        ASSERT_EQ(expected[i], d);
        ++i;
    }
}
