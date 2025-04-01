run:
	cmake --build build && ./build/tsdb

debug:
	cmake --build build gdb ./build/tsdb
