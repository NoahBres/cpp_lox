build:
	cmake -S . -B ./build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; cd ./build; make
build-run:
	cmake -S . -B ./build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; cd ./build; make; ./cpp_lox