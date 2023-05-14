cmake_minimum_required(VERSION 3.18)
project(omem VERSION 0.0.1 DESCRIPTION "OCCA based linear solver" LANGUAGES C CXX)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib64")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(3rd_party/occa.cmake)

set(SOURCES src/omem.cpp)

add_library(omem SHARED ${SOURCES})
target_link_libraries(omem PRIVATE libocca)
target_include_directories(omem PRIVATE src)
set_target_properties(omem PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1
  PUBLIC_HEADER src/omem.hpp)

add_executable(driver bin/driver.cpp)
target_link_libraries(driver PRIVATE omem)
target_include_directories(driver PRIVATE src)
set_target_properties(driver PROPERTIES VERSION ${PROJECT_VERSION})

find_program(CLANG_FORMAT NAMES clang-format)
if (CLANG_FORMAT)
  add_custom_target(format COMMAND ${CLANG_FORMAT} -i
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.[ch]
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clang-format ...")
endif()

install(TARGETS omem driver RUNTIME DESTINATION bin LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
