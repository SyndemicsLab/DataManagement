////////////////////////////////////////////////////////////////////////////////
// File: test_logging.cpp                                                     //
// Project: src                                                               //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-01                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <datamanagement/utils/logging.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include <gtest/gtest.h>

class LoggingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(LoggingTest, LogInfo) {
    const std::string LOG_NAME = "info";
    const std::string LOG_FILE = "test.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(datamanagement::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              datamanagement::utils::CreationStatus::kSuccess);
    datamanagement::utils::LogInfo(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, LogWarning) {
    const std::string LOG_NAME = "warn";
    const std::string LOG_FILE = "test.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(datamanagement::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              datamanagement::utils::CreationStatus::kSuccess);
    datamanagement::utils::LogWarning(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, LogError) {
    const std::string LOG_NAME = "error";
    const std::string LOG_FILE = "test.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(datamanagement::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              datamanagement::utils::CreationStatus::kSuccess);
    datamanagement::utils::LogError(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, LogDebug) {
    const std::string LOG_NAME = "debug";
    const std::string LOG_FILE = "test.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(datamanagement::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              datamanagement::utils::CreationStatus::kSuccess);
    datamanagement::utils::LogDebug(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}