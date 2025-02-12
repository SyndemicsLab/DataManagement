#include <datamanagement/modeldata/ModelData.hpp>
#include <gtest/gtest.h>

class ModelDataTest : public ::testing::Test {
    void SetUp() override {
        // Setup Config
        std::ofstream cf("test.conf");
        cf << "[simulation]" << std::endl;
        cf << "duration = 52" << std::endl;
        cf << "aging_interval = 260" << std::endl;
        cf << "[state]" << std::endl;
        cf << "ouds = Active_Noninjection, Active_Injection, "
              "Nonactive_Noninjection, Nonactive_Injection"
           << std::endl;
        cf.close();

        // Setup CSV
        std::ofstream csv("test.csv");
        csv << "id,name,age\n";
        csv << "1,Alice,30\n";
        csv << "2,Bob,25\n";
        csv << "3,Charlie,35\n";
        csv.close();

        // Setup DB
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

    void TearDown() override {
        std::remove("test.conf");
        std::remove("test.csv");
        std::remove("test.db");
    }
};

TEST_F(ModelDataTest, ConfigTesting) {
    datamanagement::ModelData md("test.conf");
    datamanagement::source::Config cf = md.GetConfig();

    std::string data = "";
    cf.GetFromConfig("simulation.duration", data);
    ASSERT_EQ(data, "52");
}

TEST_F(ModelDataTest, GetSourceNames) {
    datamanagement::ModelData md("test.conf");
    md.AddSource("test.db");
    md.AddSource("test.csv");

    std::vector<std::string> csvNames = md.GetCSVSourceNames();
    ASSERT_EQ(csvNames[0], "test");

    std::vector<std::string> dbNames = md.GetDBSourceNames();
    ASSERT_EQ(dbNames[0], "test");
}

TEST_F(ModelDataTest, Select) {
    datamanagement::ModelData md("test.conf");
    md.AddSource("test.db");

    std::any storage = std::vector<std::tuple<int, std::string, int>>{};

    md.GetDBSource("test").Select(
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

TEST_F(ModelDataTest, BatchExecute) {
    datamanagement::ModelData md("test.conf");
    md.AddSource("test.db");

    std::string query = "INSERT INTO test (name, age) VALUES (?, ?);";
    std::vector<std::unordered_map<int, datamanagement::source::BindingVariant>>
        batch_bindings;
    for (int i = 0; i < 10; ++i) {
        std::unordered_map<int, datamanagement::source::BindingVariant>
            bindings;
        bindings[1] = "Test" + std::to_string(i);
        bindings[2] = i;
        batch_bindings.emplace_back(bindings);
    }
    md.GetDBSource("test").BatchExecute(query, batch_bindings);

    std::any storage = std::vector<std::tuple<int, std::string, int>>{};
    md.GetDBSource("test").Select(
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
    EXPECT_EQ(std::get<1>(results[3]), "Test0");
}

TEST_F(ModelDataTest, GetData) {
    datamanagement::ModelData md("test.conf");
    md.AddSource("test.csv");

    // Test without where conditions
    Eigen::MatrixXd data = md.GetCSVSource("test").GetData({"id", "age"}, {});
    EXPECT_EQ(data.rows(), 3);
    EXPECT_EQ(data.cols(), 2);

    // Test with where conditions
    Eigen::MatrixXd filtered_data =
        md.GetCSVSource("test").GetData({"id", "age"}, {{"name", "Alice"}});
    EXPECT_EQ(filtered_data.rows(), 1);
    EXPECT_EQ(filtered_data.cols(), 2);
}

TEST_F(ModelDataTest, WriteCSV) {
    datamanagement::ModelData md("test.conf");
    md.AddSource("test.csv");

    md.GetCSVSource("test").WriteCSV("output.csv", {"id", "age"});
    datamanagement::source::CSVSource csv_source2;
    csv_source2.ConnectToFile("output.csv");

    // Test without where conditions
    Eigen::MatrixXd data1 = md.GetCSVSource("test").GetData({"id", "age"}, {});
    Eigen::MatrixXd data2 = csv_source2.GetData({"id", "age"}, {});
    ASSERT_TRUE(data2.isApprox(data1));
    std::remove("output.csv");
}