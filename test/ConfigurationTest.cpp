#include "../include/Configuration.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

class ConfigurationTest : public ::testing::Test {
protected:
    std::ofstream outStream;
    std::filesystem::path tempFilePath = std::filesystem::temp_directory_path();

    void SetUp() override {
        tempFilePath /= std::tmpnam(nullptr) + std::string(".conf");
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
    int retVal = 0;
    retVal = std::get<int>(config.get("section.value", retVal));
    EXPECT_EQ(120, retVal);
}

TEST_F(ConfigurationTest, ParseVectorOfInts) {
    outStream << "[section]\nnumbers = 10, 20, 30, 40, 50" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::string numbers = "";
    numbers = std::get<std::string>(config.get("section.numbers", numbers));
    std::vector<int> expected = {10, 20, 30, 40, 50};
    std::vector<Data::ReturnType> returned = config.parse(numbers);
    std::vector<int> casted_returned;
    std::for_each(returned.begin(), returned.end(), [&](Data::ReturnType &n) {
        casted_returned.push_back(std::get<int>(n));
    });
    EXPECT_EQ(expected, casted_returned);
}

TEST_F(ConfigurationTest, ParseVectorOfStrings) {
    outStream << "[section]\nstrings = foo, bar, baz, bat" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::string strings = "";
    strings = std::get<std::string>(config.get("section.strings", strings));
    std::vector<std::string> expected = {"foo", "bar", "baz", "bat"};
    std::vector<Data::ReturnType> returned = config.parse(strings);
    std::vector<std::string> casted_returned;
    std::for_each(returned.begin(), returned.end(), [&](Data::ReturnType &n) {
        casted_returned.push_back(std::get<std::string>(n));
    });
    EXPECT_EQ(expected, casted_returned);
}

TEST_F(ConfigurationTest, OptionalValueProvided) {
    outStream << "[section]\noptional = 11235813" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::shared_ptr<int> optional = std::make_shared<int>(0);
    std::shared_ptr<Data::ReturnType> temp =
        config.get_optional("section.optional", *optional);
    EXPECT_NE(temp, nullptr);
    *optional = std::get<int>(*temp);
    EXPECT_EQ(11235813, *optional);
}

TEST_F(ConfigurationTest, OptionalIntNotProvided) {
    outStream << "[section]\noptional =" << std::endl;
    Data::Configuration config(tempFilePath.string());

    std::shared_ptr<int> optional = std::make_shared<int>(0);
    std::shared_ptr<Data::ReturnType> temp =
        config.get_optional("section.optional", *optional);
    EXPECT_EQ(nullptr, temp);

    std::shared_ptr<std::string> optional2 = std::make_shared<std::string>("");
    std::shared_ptr<Data::ReturnType> temp2 =
        config.get_optional("section.optional", *optional2);
    EXPECT_EQ(nullptr, temp2);
}

TEST_F(ConfigurationTest, OptionalStringNotProvided) {
    outStream << "[section]\noptional =" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::shared_ptr<std::string> optional = std::make_shared<std::string>("");
    std::shared_ptr<Data::ReturnType> temp =
        config.get_optional("section.optional", *optional);
    EXPECT_EQ(nullptr, temp);
}

TEST_F(ConfigurationTest, GetStringVector) {
    outStream << "[section]\nvector = foo, bar, baz" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::vector<Data::ReturnType> returned = config.getVector("section.vector");
    std::vector<std::string> casted_returned;
    std::for_each(returned.begin(), returned.end(), [&](Data::ReturnType &n) {
        casted_returned.push_back(std::get<std::string>(n));
    });

    std::vector<std::string> expected = {"foo", "bar", "baz"};
    EXPECT_EQ(expected, casted_returned);
}

TEST_F(ConfigurationTest, GetIntVector) {
    outStream << "[section]\nvector = 1, 2, 3" << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::vector<Data::ReturnType> returned = config.getVector("section.vector");
    std::vector<int> casted_returned;
    std::for_each(returned.begin(), returned.end(), [&](Data::ReturnType &n) {
        casted_returned.push_back(std::get<int>(n));
    });
    std::vector<int> expected = {1, 2, 3};
    EXPECT_EQ(expected, casted_returned);
}

TEST_F(ConfigurationTest, GetNoVector) {
    outStream << "[section]\nvector =" << std::endl;
    Data::Configuration config(tempFilePath.string());
    EXPECT_TRUE(config.getVector("section.vector").empty());
}

TEST_F(ConfigurationTest, GetSectionCategories) {
    outStream << "[section]\nvector = foo, bar, baz\nvectortwo = aaa, bbb, ccc "
              << std::endl;
    Data::Configuration config(tempFilePath.string());
    std::vector<std::string> expected = {"vector", "vectortwo"};
    EXPECT_EQ(expected, config.getSectionCategories("section"));
}
