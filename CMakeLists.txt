cmake_minimum_required(VERSION 3.0.0)

project(pdf_text_extraction)

set (CMAKE_CXX_STANDARD 11)

set(LOG OFF CACHE BOOL "Enable debug print statments" FORCE)
if (LOG)
  set(CXX_FLAGS "${CXX_FLAGS} -DLOG")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# # Instruct CMake to run moc automatically when needed
# set(CMAKE_AUTOMOC ON)
# # Create code from a list of Qt designer ui files
# set(CMAKE_AUTOUIC ON)

set (sources
  main.cpp
  # main_test.cpp
#  tinyxml2.cpp
  )

set (hummus_path $ENV{HOME}/toolchains/hummus/)
include_directories(${hummus_path}/include)

add_executable(a.out ${sources})

target_link_libraries(a.out
  ${hummus_path}/lib/libPDFWriter.a
  ${hummus_path}/lib/libFreeType.a
  ${hummus_path}/lib/libLibAesgm.a
  ${hummus_path}/lib/libLibPng.a
  ${hummus_path}/lib/libLibTiff.a
  ${hummus_path}/lib/libLibJpeg.a
  ${hummus_path}/lib/libZlib.a
  )

# Tests
add_executable(test1
  tests/main.cpp
  tests/test1.cpp
  )

target_link_libraries(test1
  ${hummus_path}/lib/libPDFWriter.a
  ${hummus_path}/lib/libFreeType.a
  ${hummus_path}/lib/libLibAesgm.a
  ${hummus_path}/lib/libLibPng.a
  ${hummus_path}/lib/libLibTiff.a
  ${hummus_path}/lib/libLibJpeg.a
  ${hummus_path}/lib/libZlib.a
  )
