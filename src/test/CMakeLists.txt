find_package(Catch2 REQUIRED)

link_directories(
  ${vol_BINARY_DIR}/src/main
  ${vol_BINARY_DIR}/src/test
  /usr/local/lib
)

include_directories(
  ${vol_SOURCE_DIR}/src/main
  ${vol_SOURCE_DIR}/src/test
  ${APPLE_CPP_ROOT}/usr/include
  /usr/local/include
)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  vol-tests 
   
  market-test.cpp
  simulation-test.cpp
)

target_link_libraries(vol-tests Catch2::Catch2 sim market)


