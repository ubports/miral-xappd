project(MirAl-Xappd)
cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig)
#pkg_check_modules(MIRAL REQUIRED miral>=1.5)

add_subdirectory(src/)
