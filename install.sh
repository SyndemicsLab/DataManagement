rm -rf build/*
cmake -S . build/ -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build/ --target install