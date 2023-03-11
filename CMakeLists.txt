cmake_minimum_required(VERSION 3.20)
project(OPAL VERSION 1.0.0 LANGUAGES CXX)

# Define configurable options.
set(BUILD_DEMOS true CACHE BOOL "Build demonstration executables.")

# Discover sources.
file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl"
)

# Declare the library.
add_library(${PROJECT_NAME} INTERFACE ${TARGET_SOURCES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set some target options.
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	OUTPUT_NAME "opal"
)

# Publish the headers.
target_include_directories(${PROJECT_NAME} INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Build the demos.
# (After the library target was defined.)
if(${BUILD_DEMOS})
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/demos")
endif()
