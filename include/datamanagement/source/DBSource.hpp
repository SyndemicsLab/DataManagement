#ifndef DBDATASOURCE_HPP_
#define DBDATASOURCE_HPP_

#include <SQLiteCpp/SQLiteCpp.h>
#include <any>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace datamanagement::source {
    using BindingVariant = std::variant<int, double, std::string>;
    class DBSource {
    private:
        SQLite::Database db;
        std::string name = "";

    public:
        DBSource(const std::string &path)
            : db(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
            std::filesystem::path p = path;
            name = p.filename().string();
        }
        ~DBSource() = default;

        // Move Constructor
        DBSource(DBSource &&old) : db(std::move(old.db)), name(old.name) {}

        std::string GetName() const { return name; }

        void
        Select(const std::string &query,
               std::function<void(std::any &storage,
                                  const SQLite::Statement &stmt)>
                   callback,
               std::any &storage,
               const std::unordered_map<int, BindingVariant> &bindings = {}) {
            try {
                SQLite::Statement stmt(db, query);

                for (const auto &[index, value] : bindings) {
                    if (value.index() == 0) {
                        stmt.bind(index, std::get<int>(value));
                    } else if (value.index() == 1) {
                        stmt.bind(index, std::get<double>(value));
                    } else {
                        stmt.bind(index, std::get<std::string>(value));
                    }
                }

                SQLite::Transaction transaction(db);

                while (stmt.executeStep()) {
                    callback(storage, stmt);
                }

                transaction.commit();
            } catch (const std::exception &e) {
                throw std::runtime_error("Error executing query: " + query +
                                         "\n" + e.what());
            }
        }

        void
        BatchExecute(const std::string &query,
                     const std::vector<std::unordered_map<int, BindingVariant>>
                         &bindings_batch = {}) {
            try {
                SQLite::Transaction transaction(db);
                SQLite::Statement stmt(db, query);

                for (auto &bindings : bindings_batch) {
                    for (const auto &[index, value] : bindings) {
                        if (value.index() == 0) {
                            stmt.bind(index, std::get<int>(value));
                        } else if (value.index() == 1) {
                            stmt.bind(index, std::get<double>(value));
                        } else {
                            stmt.bind(index, std::get<std::string>(value));
                        }
                    }
                    stmt.exec();
                    stmt.reset();
                }
                transaction.commit();

            } catch (const std::exception &e) {
                throw std::runtime_error("Error executing query: " + query +
                                         "\n" + e.what());
            }
        }
    };
} // namespace datamanagement::source

#endif