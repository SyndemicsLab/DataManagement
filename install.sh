rm -rf build/*
cmake -S . -B build -DBUILD_SHARED_LIBS=YES -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix dminstall
