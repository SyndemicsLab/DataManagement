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

TEST_F(ConfigurationTest, ParseSingleValue) {
    outStream << "[section]\nvalue = 120" << std::endl;
    Data::Configuration config(tempFilePath.string());
    EXPECT_EQ(120, config.get<int>("section.value"));
}

TEST_F(ConfigurationTest, ParseVectorOfInts) {
    outStream << "[section]\nnumbers = 10, 20, 30, 40, 50" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::string numbers = config.get<std::string>("section.numbers");
    std::vector<int> expected = {10, 20, 30, 40, 50};
    EXPECT_EQ(expected, config.parseString2VectorOfInts(numbers));
}

TEST_F(ConfigurationTest, ParseVectorOfStrings) {
    outStream << "[section]\nstrings = foo, bar, baz, bat" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::string strings = config.get<std::string>("section.strings");
    std::vector<std::string> expected = {"foo", "bar", "baz", "bat"};
    EXPECT_EQ(expected, config.parseString2VectorOfStrings(strings));
}

TEST_F(ConfigurationTest, OptionalValueProvided) {
    outStream << "[section]\noptional = 11235813" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::shared_ptr<int> optional = config.optional<int>("section.optional");
    EXPECT_EQ(11235813, *optional);
}

TEST_F(ConfigurationTest, OptionalIntNotProvided) {
    outStream << "[section]\noptional =" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::shared_ptr<int> optional = config.optional<int>("section.optional");
    std::shared_ptr<std::string> optional2 =
        config.optional<std::string>("section.optional");
    EXPECT_EQ(nullptr, optional);
    EXPECT_EQ(nullptr, optional);
}

TEST_F(ConfigurationTest, OptionalStringNotProvided) {
    outStream << "[section]\noptional =" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::shared_ptr<std::string> optional =
        config.optional<std::string>("section.optional");
    std::shared_ptr<std::string> expected = nullptr;
    EXPECT_EQ(expected, optional);
}
