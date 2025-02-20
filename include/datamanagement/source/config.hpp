////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: source                                                            //
// Created Date: Th Feb 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: Thu Feb 20 2025                                             //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
// -----                                                                      //
// HISTORY:                                                                   //
// Date      	By	Comments                                                  //
// ----------	---	--------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATAMANAGEMENT_SOURCE_CONFIG_HPP_
#define DATAMANAGEMENT_SOURCE_CONFIG_HPP_

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace datamanagement::source {
class Config {
private:
    boost::property_tree::ptree ptree;

public:
    Config(const std::string &path) { read_ini(path, ptree); }
    ~Config() = default;

    void GetFromConfig(std::string const key, std::string &data) const {
        try {
            data = ptree.get<std::string>(key);
        } catch (const std::exception &e) {
            // log bad cast
        }
    }

    void GetConfigSectionCategories(std::string const section,
                                    std::vector<std::string> &data) const {
        boost::property_tree::ptree sub_tree = this->ptree.get_child(section);
        std::vector<std::string> key_list;

        for (auto &key : sub_tree) {
            key_list.push_back(key.first);
        }
        data = key_list;
    }
};
} // namespace datamanagement::source

#endif // DATAMANAGEMENT_SOURCE_CONFIG_HPP_