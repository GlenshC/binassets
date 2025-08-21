.PHONY: release debug clean clang gcc msvc build config writer reader

debug:
	cmake -G Ninja -B build -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Debug -DBINASSETS_BUILD_EXE=ON
	cmake --build build

writer:
	cmake -G Ninja -B build -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Debug -DBINASSETS_BUILD_EXE=ON -DBINASSETS_WRITER_MAIN=ON
	cmake --build build
reader:
	cmake -G Ninja -B build -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Debug -DBINASSETS_BUILD_EXE=ON -DBINASSETS_WRITER_MAIN=OFF
	cmake --build build

config:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DBINASSETS_BUILD_EXE=ON

build:
	cmake --build build

clang: 
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DBINASSETS_BUILD_EXE=ON
	cmake --build build --config Release

gcc:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DBINASSETS_BUILD_EXE=ON
	cmake --build build --config Release

msvc:
	cmake -G "Visual Studio 17 2022" -A x64 -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DBINASSETS_BUILD_EXE=ON
	cmake --build build --config Release

release:
	cmake -Bbuild -A X64 -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

clean:
	rm -rf build
