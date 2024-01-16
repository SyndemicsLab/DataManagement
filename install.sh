rm -rf build/*
cmake -S . build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/
sudo cmake --install build/