# build
b:
	CC=/opt/homebrew/Cellar/llvm/16.0.6/bin/clang CXX=/opt/homebrew/Cellar/llvm/16.0.6/bin/clang++ cmake -S . -B ./build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; cd ./build; make
# build run
br:
	CC=/opt/homebrew/Cellar/llvm/16.0.6/bin/clang CXX=/opt/homebrew/Cellar/llvm/16.0.6/bin/clang++ cmake -S . -B ./build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; cd ./build; make; ./cpp_lox
