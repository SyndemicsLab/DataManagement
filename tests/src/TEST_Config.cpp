#include <datamanagement/source/Config.hpp>
#include <gtest/gtest.h>

class ConfigTest : public ::testing::Test {
    void SetUp() override {
        std::ofstream file("test.conf");
        file << "[simulation]" << std::endl;
        file << "duration = 52" << std::endl;
        file << "aging_interval = 260" << std::endl;
        file << "[state]" << std::endl;
        file << "ouds = Active_Noninjection, Active_Injection, "
                "Nonactive_Noninjection, Nonactive_Injection"
             << std::endl;
        file.close();
    }

    void TearDown() override { std::remove("test.conf"); }
};

TEST_F(ConfigTest, GetFromConfig) {
    datamanagement::source::Config config("test.conf");
    std::string data = "";
    config.GetFromConfig("simulation.duration", data);
    ASSERT_EQ(data, "52");
}

TEST_F(ConfigTest, GetConfigSectionCategories) {
    datamanagement::source::Config config("test.conf");
    std::vector<std::string> data = {};
    config.GetConfigSectionCategories("simulation", data);
    std::vector<std::string> expected = {"duration", "aging_interval"};
    int i = 0;
    for (std::string d : data) {
        ASSERT_EQ(expected[i], d);
        ++i;
    }
}
