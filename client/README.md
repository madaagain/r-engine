# R-TYPE--MIRROR : Client

How to build and execute the client ?

Start from the root of the repository.
Then execute the following commands:
```bash
[bash]> cd client/
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
[bash]> ./r-type_client
```

You'll `see r-type_client`, now do :
```bash
[bash]> ./r-type_client
```

<!-- sudo dnf install systemd-devel -->
<!-- type_index datatype le plus léger -->
