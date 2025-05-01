////////////////////////////////////////////////////////////////////////////////
// File: test_db_source.cpp                                                    //
// Project: src                                                               //
// Created Date: Th Feb 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-01                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
// -----                                                                      //
// HISTORY:                                                                   //
// Date      	By	Comments                                                  //
// ----------	---	--------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <tuple>

#include <datamanagement/source/db_source.hpp>

using db_vec = std::vector<std::tuple<int, std::string, int, double>>;

class DBSourceTest : public ::testing::Test {
protected:
    void SetUp() override {
        SQLite::Database db("test.db",
                            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db.exec("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name "
                "TEXT, age INTEGER, score REAL);");
        SQLite::Transaction transaction(db);
        db.exec(
            "INSERT INTO test (name, age, score) VALUES ('Alice', 30, 2.45);");
        db.exec(
            "INSERT INTO test (name, age, score) VALUES ('Bob', 25, 1.23);");
        db.exec("INSERT INTO test (name, age, score) VALUES ('Charlie', 35, "
                "7.54);");
        transaction.commit();
    }
    void TearDown() override { std::remove("test.db"); }
};

TEST_F(DBSourceTest, Select) {
    datamanagement::source::DBSource db_source;
    db_source.SetDatabasePath("test.db");

    std::any storage = db_vec{};

    db_source.Select(
        "SELECT * FROM test;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            db_vec *results = std::any_cast<db_vec>(&storage);
            results->emplace_back(
                stmt.getColumn(0).getInt(), stmt.getColumn(1).getText(),
                stmt.getColumn(2).getInt(), stmt.getColumn(3).getInt());
        },
        storage);

    db_vec results = std::any_cast<db_vec>(storage);

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[1]), "Bob");
    EXPECT_EQ(std::get<1>(results[2]), "Charlie");
}

TEST_F(DBSourceTest, SelectWithBindings) {
    datamanagement::source::DBSource db_source;
    db_source.SetDatabasePath("test.db");
    std::any storage = db_vec{};

    std::unordered_map<int, datamanagement::source::BindingVariant> bindings;
    bindings[1] = 1;
    db_source.Select(
        "SELECT * FROM test WHERE id = ?;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            db_vec *results = std::any_cast<db_vec>(&storage);
            results->emplace_back(
                stmt.getColumn(0).getInt(), stmt.getColumn(1).getText(),
                stmt.getColumn(2).getInt(), stmt.getColumn(3).getDouble());
        },
        storage, bindings);

    db_vec results = std::any_cast<db_vec>(storage);

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

TEST_F(DBSourceTest, SelectWithDoubleBindings) {
    datamanagement::source::DBSource db_source;
    db_source.SetDatabasePath("test.db");
    std::any storage = db_vec{};

    std::unordered_map<int, datamanagement::source::BindingVariant> bindings;
    bindings[1] = 2.45;
    db_source.Select(
        "SELECT * FROM test WHERE score = ?;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            db_vec *results = std::any_cast<db_vec>(&storage);
            results->emplace_back(
                stmt.getColumn(0).getInt(), stmt.getColumn(1).getText(),
                stmt.getColumn(2).getInt(), stmt.getColumn(3).getDouble());
        },
        storage, bindings);

    db_vec results = std::any_cast<db_vec>(storage);

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

TEST_F(DBSourceTest, SelectWithStringBindings) {
    datamanagement::source::DBSource db_source;
    db_source.SetDatabasePath("test.db");
    std::any storage = db_vec{};

    std::unordered_map<int, datamanagement::source::BindingVariant> bindings;
    bindings[1] = "Alice";
    db_source.Select(
        "SELECT * FROM test WHERE name = ?;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            db_vec *results = std::any_cast<db_vec>(&storage);
            results->emplace_back(
                stmt.getColumn(0).getInt(), stmt.getColumn(1).getText(),
                stmt.getColumn(2).getInt(), stmt.getColumn(3).getDouble());
        },
        storage, bindings);

    db_vec results = std::any_cast<db_vec>(storage);

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

TEST_F(DBSourceTest, BatchExecute) {
    datamanagement::source::DBSource db_source;
    db_source.SetDatabasePath("test.db");
    std::string query = "INSERT INTO test (name, age, score) VALUES (?, ?, ?);";
    std::vector<std::unordered_map<int, datamanagement::source::BindingVariant>>
        batch_bindings;
    for (int i = 0; i < 10; ++i) {
        std::unordered_map<int, datamanagement::source::BindingVariant>
            bindings;
        bindings[1] = "Test" + std::to_string(i);
        bindings[2] = i;
        bindings[3] = i + 0.3;
        batch_bindings.emplace_back(bindings);
    }
    db_source.BatchExecute(query, batch_bindings);

    std::any storage = db_vec{};
    db_source.Select(
        "SELECT * FROM test;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            db_vec *results = std::any_cast<db_vec>(&storage);
            results->emplace_back(
                stmt.getColumn(0).getInt(), stmt.getColumn(1).getText(),
                stmt.getColumn(2).getInt(), stmt.getColumn(3).getDouble());
        },
        storage);
    db_vec results = std::any_cast<db_vec>(storage);
    EXPECT_EQ(results.size(), 13);
    EXPECT_EQ(std::get<1>(results[3]), "Test0");
}
