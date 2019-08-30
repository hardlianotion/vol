link_directories(
  ${vol_BINARY_DIR}/src/main
  ${vol_BINARY_DIR}/src/test
  ${APPLE_CPP_ROOT}/usr/lib
  /usr/local/lib
)

include_directories(
  ${vol_SOURCE_DIR}/src/test
  ${APPLE_CPP_ROOT}/usr/include
  /usr/local/include
)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
  market 
   
  market/market.cpp
)

add_library(
  sim
   
  sim/simulation.cpp
)

add_executable (
  vol-demo

  main.cpp
)

target_link_libraries(vol-demo market sim)


