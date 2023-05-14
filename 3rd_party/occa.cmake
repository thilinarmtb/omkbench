include(FetchContent)

FetchContent_Declare(occa_content
  GIT_REPOSITORY https://github.com/thilinarmtb/occa.git
  GIT_TAG replace_cmake_source_dir
)

FetchContent_GetProperties(occa_content)
if (NOT occa_content_POPULATED)
  FetchContent_Populate(occa_content)
endif()

set(OCCA_SOURCE_DIR "${occa_content_SOURCE_DIR}")
add_subdirectory(${occa_content_SOURCE_DIR} ${occa_content_BINARY_DIR})
