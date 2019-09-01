link_directories(
  ${vol_BINARY_DIR}/src/main
  /usr/local/lib
)

include_directories(
  ${vol_SOURCE_DIR}/src/main/
  ${range-v3_DIR}/include
  /usr/local/include
)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
  vol 
   
  market/market.cpp
  sim/simulation.cpp
)

target_link_libraries(vol range-v3)

add_executable (
  vol-demo

  main.cpp
)

target_link_libraries(vol-demo vol)

