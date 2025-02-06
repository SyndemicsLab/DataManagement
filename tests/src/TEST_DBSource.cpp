#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <tuple>

#include <datamanagement/source/DBSource.hpp>

class DBSourceTest : public ::testing::Test {
protected:
    void SetUp() override {
        SQLite::Database db("test.db",
                            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db.exec("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name "
                "TEXT, age INTEGER);");
        SQLite::Transaction transaction(db);
        db.exec("INSERT INTO test (name, age) VALUES ('Alice', 30);");
        db.exec("INSERT INTO test (name, age) VALUES ('Bob', 25);");
        db.exec("INSERT INTO test (name, age) VALUES ('Charlie', 35);");
        transaction.commit();
    }
    void TearDown() override { std::remove("test.db"); }
};

TEST_F(DBSourceTest, Select) {
    datamanagement::source::DBSource db_source("test.db");

    std::any storage = std::vector<std::tuple<int, std::string, int>>{};

    db_source.Select(
        "SELECT * FROM test;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            std::vector<std::tuple<int, std::string, int>> *results =
                std::any_cast<std::vector<std::tuple<int, std::string, int>>>(
                    &storage);
            results->emplace_back(stmt.getColumn(0).getInt(),
                                  stmt.getColumn(1).getText(),
                                  stmt.getColumn(2).getInt());
        },
        storage);

    std::vector<std::tuple<int, std::string, int>> results =
        std::any_cast<std::vector<std::tuple<int, std::string, int>>>(storage);

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[1]), "Bob");
    EXPECT_EQ(std::get<1>(results[2]), "Charlie");
}

TEST_F(DBSourceTest, SelectWithBindings) {
    datamanagement::source::DBSource db_source("test.db");
    std::any storage = std::vector<std::tuple<int, std::string, int>>{};

    std::unordered_map<int, datamanagement::source::BindingVariant> bindings;
    bindings[1] = 1;
    db_source.Select(
        "SELECT * FROM test WHERE id = ?;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            std::vector<std::tuple<int, std::string, int>> *results =
                std::any_cast<std::vector<std::tuple<int, std::string, int>>>(
                    &storage);
            results->emplace_back(stmt.getColumn(0).getInt(),
                                  stmt.getColumn(1).getText(),
                                  stmt.getColumn(2).getInt());
        },
        storage, bindings);

    std::vector<std::tuple<int, std::string, int>> results =
        std::any_cast<std::vector<std::tuple<int, std::string, int>>>(storage);

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

TEST_F(DBSourceTest, BatchExecute) {
    datamanagement::source::DBSource db_source("test.db");
    std::string query = "INSERT INTO test (name, age) VALUES (?, ?);";
    std::vector<std::unordered_map<int, datamanagement::source::BindingVariant>>
        batch_bindings;
    for (int i = 0; i < 10; ++i) {
        std::unordered_map<int, datamanagement::source::BindingVariant>
            bindings;
        bindings[1] = i;
        bindings[2] = "Test" + std::to_string(i);
        batch_bindings.emplace_back(bindings);
    }
    db_source.BatchExecute(query, batch_bindings);

    std::any storage = std::vector<std::tuple<int, std::string, int>>{};
    db_source.Select(
        "SELECT * FROM test;",
        [](std::any &storage, const SQLite::Statement &stmt) {
            std::vector<std::tuple<int, std::string, int>> *results =
                std::any_cast<std::vector<std::tuple<int, std::string, int>>>(
                    &storage);
            results->emplace_back(stmt.getColumn(0).getInt(),
                                  stmt.getColumn(1).getText(),
                                  stmt.getColumn(2).getInt());
        },
        storage);
    std::vector<std::tuple<int, std::string, int>> results =
        std::any_cast<std::vector<std::tuple<int, std::string, int>>>(storage);
    EXPECT_EQ(results.size(), 13);
}
