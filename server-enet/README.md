# R-TYPE--MIRROR : server

How to build and execute the server ?

Start from the root of the repository.
Then execute the following commands:
```bash
[bash]> cd server/
[bash]> mkdir -p cmake
[bash]> wget -O cmake/CPM.cmake https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake
[bash]> mkdir -p build
[bash]> cd build
[bash]> cmake ..
```

CMake will generate makefile, then you can `make` the project and execute the binary :
```bash
[bash]> make
[bash]> cd ..
[bash]> ./r-type_server
```

You'll `see r-type_server`, now do :
```bash
[bash]> ./r-type_server
```