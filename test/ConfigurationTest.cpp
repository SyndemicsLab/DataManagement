#include "Configuration.hpp"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

class ConfigurationTest : public ::testing::Test {
protected:
    std::ofstream outStream;
    boost::filesystem::path tempFilePath =
        boost::filesystem::temp_directory_path();

    void SetUp() override {
        tempFilePath /= boost::filesystem::unique_path("%%%%.conf");
        outStream.open(tempFilePath);
    }

    void TearDown() override {
        if (outStream.is_open()) {
            outStream.close();
        }
    }
};

TEST_F(ConfigurationTest, ParseConfig) {
    outStream << "[simulation]\nduration = 120" << std::endl;
    Data::Configuration config(tempFilePath.string());
    EXPECT_EQ(120, config.get<int>("simulation.duration"));
}
