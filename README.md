# Data Management

Data Management Tool used to handle all input/output for Syndemics Lab Models. Contains classes describing a Data Table and configuration files following the standard `ini` file type.

## Building The Library

### Dependencies

- [CMake](https://cmake.org/)
- [Boost Lexical Cast](https://www.boost.org/doc/libs/1_36_0/libs/conversion/lexical_cast.htm)
- [Boost Property Tree](https://www.boost.org/doc/libs/1_81_0/doc/html/property_tree.html)
- [GoogleTest/`gtest`](https://github.com/google/googletest) (optional)

### Clone and Build via CMake

```bat
git clone git@github.com:SyndemicsLab/DataManagement.git
cd DataManagement
cmake -B build/ -DCMAKE_BUILD_TYPE=Release -DBUILD_DATA_TESTS=OFF
cmake --build build/ --config Release
```

## Data Types

### Tabular Data

Tabular data is handled in a DataTable object. DataTables are made up of rows and columns. DataTable has a constructor intended to handle Comma-Separated-Value (CSV) files. There is future plans to also implement a way to read from an SQL Table into a DataTable object.

### Configuration Data

Configuration file data is read via the Configuration object. It parses an expected ini file making use of the boost property tree and allows for optional parameters and a way to parse apart strings into vectors.
