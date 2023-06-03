mkdir build
cd build
g++-13 -std=c++20 -fmodules-ts -xc++-system-header iostream
cmake ../CMakeLists.txt
