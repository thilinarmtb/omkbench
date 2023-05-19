include(FetchContent)

FetchContent_Declare(occa_content
  GIT_REPOSITORY https://github.com/libocca/occa.git
  GIT_TAG development
)

FetchContent_GetProperties(occa_content)
if (NOT occa_content_POPULATED)
  FetchContent_Populate(occa_content)
endif()

set(OCCA_SOURCE_DIR "${occa_content_SOURCE_DIR}")
add_subdirectory(${occa_content_SOURCE_DIR} ${occa_content_BINARY_DIR})
