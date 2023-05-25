cmake_minimum_required(VERSION 3.23)
project(omk VERSION 0.0.1 DESCRIPTION "OCCA based linear solver" LANGUAGES C CXX)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib64")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(3rd_party/occa.cmake)

set(SOURCES src/omk.cpp src/omk-bench.cpp)

add_library(omk SHARED ${SOURCES})
set_target_properties(omk PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1
  PUBLIC_HEADER src/omk.hpp)
target_link_libraries(omk PRIVATE libocca)
target_include_directories(omk PRIVATE src)
target_compile_options(omk PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

add_executable(driver bin/driver.cpp)
set_target_properties(driver PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(driver PRIVATE omk)
target_include_directories(driver PRIVATE src)
target_compile_options(driver PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

find_program(CLANG_FORMAT NAMES clang-format)
if (CLANG_FORMAT)
  add_custom_target(format COMMAND ${CLANG_FORMAT} -i
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.[ch]
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clang-format ...")
endif()

install(DIRECTORY okl/ DESTINATION okl FILES_MATCHING REGEX "\.okl$|\.c|\.h$|\.hpp$|")
install(TARGETS omk driver RUNTIME DESTINATION bin LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
