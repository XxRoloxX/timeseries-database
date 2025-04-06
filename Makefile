run:
	cmake --build build && ./build/tsdb

test:
	cmake --build build && cd build && ctest --output-on-failure

debug:
	cmake --build build gdb ./build/tsdb
