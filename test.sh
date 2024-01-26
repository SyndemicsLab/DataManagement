rm -rf build/*
cmake -S . -B build -DBUILD_SHARED_LIBS=YES -DCMAKE_BUILD_TYPE=Debug -DBUILD_DATA_TESTS=ON
cmake --build build
./build/test/dataTests