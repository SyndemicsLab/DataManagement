cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 17)

project(
    DataManagement
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

add_library(data
    src/DataTable.cpp
)

set(DM_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(data PUBLIC ${DM_INCLUDES})
