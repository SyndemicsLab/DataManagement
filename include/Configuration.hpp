#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <memory>
#include <string>
#include <variant>
#include <vector>

/// @brief Namespace defining Data Containers
namespace Data {

    class PTree;

    class IConfiguration {
    public:
        virtual ~IConfiguration() = default;
        // virtual void get(std::string str,
        //                  std::variant<int, bool, float, double, char,
        //                               std::string> &value) = 0;
        // virtual void
        // optional(std::string str,
        //          std::variant<std::shared_ptr<int>, std::shared_ptr<bool>,
        //                       std::shared_ptr<float>,
        //                       std::shared_ptr<double>, std::shared_ptr<char>,
        //                       std::shared_ptr<std::string>> &value) = 0;
        virtual std::vector<std::string>
        parseString2VectorOfStrings(std::string st) = 0;
        virtual std::vector<int> parseString2VectorOfInts(std::string st) = 0;
        virtual std::vector<std::string> getStringVector(std::string str) = 0;
        virtual std::vector<int> getIntVector(std::string str) = 0;
        virtual std::vector<std::string>
        getSectionCategories(std::string section) = 0;
    };

    using IConfigurationPtr = std::shared_ptr<Data::IConfiguration>;

    /// @brief Class describing a standard configuration file
    class Configuration : public IConfiguration {
    private:
        std::unique_ptr<PTree> dmTree;

    public:
        // CONSTRUCTORS

        /// @brief Default constructor with no file
        Configuration();

        /// @brief Primary Constructor used with a file
        /// @param configFile string path to the config file
        Configuration(std::string configFile);

        /// @brief Default destructor
        ~Configuration();

        /// @brief Default template for getting parameters from the config
        /// @param T Type to return
        /// @param str String key to search for
        /// @return The value searched for of type T
        template <typename T> T get(std::string str);

        /// @brief Template for the optional parameters
        /// @param T Type to return
        /// @param str String key to search for
        /// @return The optional value searched for of type T
        template <typename T> std::shared_ptr<T> optional(std::string str);

        // /// @brief
        // /// @param str
        // /// @param value
        // void get(std::string str,
        //          std::variant<int, bool, float, double, char, std::string>
        //              &value) override;

        // /// @brief An overload of \code{optional} for strings
        // /// @param str String key to search for
        // /// @return The optional value searched for of type string
        // void
        // optional(std::string str,
        //          std::variant<std::shared_ptr<int>, std::shared_ptr<bool>,
        //                       std::shared_ptr<float>,
        //                       std::shared_ptr<double>, std::shared_ptr<char>,
        //                       std::shared_ptr<std::string>> &value) override;

        /// @brief Helper function to parse a string to a vector of strings
        /// @param st string to parse
        /// @return vector of parsed strings
        std::vector<std::string>
        parseString2VectorOfStrings(std::string st) override;

        /// @brief Helper function to parse a string to a vector of ints
        /// @param st string to parse
        /// @return vector of parsed ints
        std::vector<int> parseString2VectorOfInts(std::string st) override;

        /// @brief Wrapper function around `optional` and
        /// `parseString2VectorofStrings`.
        /// @param str string to parse
        /// @return vector of parsed strings
        std::vector<std::string> getStringVector(std::string str) override {
            std::shared_ptr<std::string> value =
                this->optional<std::string>(str);
            if (value) {
                return this->parseString2VectorOfStrings(*value);
            } else {
                return {};
            }
        }

        /// @brief Wrapper function around `optional` and
        /// `parseString2VectorofInts`.
        /// @param str string to parse
        /// @return vector of parsed ints
        std::vector<int> getIntVector(std::string str) override {
            std::shared_ptr<std::string> value =
                this->optional<std::string>(str);
            if (value) {
                return this->parseString2VectorOfInts(*value);
            } else {
                return {};
            }
        }

        std::vector<std::string>
        getSectionCategories(std::string section) override;
    };
} // namespace Data
#endif
