add_library(mxd socket.cpp xserver.cpp)
target_include_directories (mxd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
  miral-xappd
  main.cpp
)

target_link_libraries (miral-xappd LINK_PUBLIC mxd)
