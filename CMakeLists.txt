cmake_minimum_required(VERSION 3.0.0)

project(pdf_text_extraction)

enable_testing()

set (CMAKE_CXX_STANDARD 11)

set(LOG OFF CACHE BOOL "Enable debug print statments" FORCE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (sources
  main.cpp
)

set (HUMMUS_PATH $ENV{HOME}/toolchains/hummus/ CACHE STRING "Path to PDF-Writer libraries" )
include_directories(${HUMMUS_PATH}/include)

add_executable(a.out ${sources})

target_link_libraries(a.out
  ${HUMMUS_PATH}/lib/libPDFWriter.a
  ${HUMMUS_PATH}/lib/libFreeType.a
  ${HUMMUS_PATH}/lib/libLibAesgm.a
  ${HUMMUS_PATH}/lib/libLibPng.a
  ${HUMMUS_PATH}/lib/libLibTiff.a
  ${HUMMUS_PATH}/lib/libLibJpeg.a
  ${HUMMUS_PATH}/lib/libZlib.a
  )

# Tests
add_executable(test1
  tests/main.cpp
  tests/test1.cpp
  )
add_test(NAME test1 COMMAND test1)

add_executable(test_traverse_nodes
  tests/main.cpp
  tests/test_traverse_nodes.cpp
  )
add_test(NAME test_traverse_nodes COMMAND test_traverse_nodes)

add_executable(test2
  tests/main.cpp
  tests/test2.cpp
  )
add_test(NAME test2 COMMAND test2)

target_link_libraries(test1
  ${HUMMUS_PATH}/lib/libPDFWriter.a
  ${HUMMUS_PATH}/lib/libFreeType.a
  ${HUMMUS_PATH}/lib/libLibAesgm.a
  ${HUMMUS_PATH}/lib/libLibPng.a
  ${HUMMUS_PATH}/lib/libLibTiff.a
  ${HUMMUS_PATH}/lib/libLibJpeg.a
  ${HUMMUS_PATH}/lib/libZlib.a
  )

target_link_libraries(test_traverse_nodes
  ${HUMMUS_PATH}/lib/libPDFWriter.a
  ${HUMMUS_PATH}/lib/libFreeType.a
  ${HUMMUS_PATH}/lib/libLibAesgm.a
  ${HUMMUS_PATH}/lib/libLibPng.a
  ${HUMMUS_PATH}/lib/libLibTiff.a
  ${HUMMUS_PATH}/lib/libLibJpeg.a
  ${HUMMUS_PATH}/lib/libZlib.a
  )

target_link_libraries(test2
  ${HUMMUS_PATH}/lib/libPDFWriter.a
  ${HUMMUS_PATH}/lib/libFreeType.a
  ${HUMMUS_PATH}/lib/libLibAesgm.a
  ${HUMMUS_PATH}/lib/libLibPng.a
  ${HUMMUS_PATH}/lib/libLibTiff.a
  ${HUMMUS_PATH}/lib/libLibJpeg.a
  ${HUMMUS_PATH}/lib/libZlib.a
  )
