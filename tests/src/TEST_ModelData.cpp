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