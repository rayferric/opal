cmake_minimum_required(VERSION 3.20)
project(OPALDemo VERSION 1.0.0 LANGUAGES CXX)

# Set output directory to "bin".
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin")
endforeach()

# Each .cpp file in the root directory is a separate demo.
file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

function(kebab_to_pascal OUTPUT_STRING INPUT_STRING)
	string(REPLACE "-" ";" SPLIT_STRING ${INPUT_STRING})
	set(PASCAL_CASE "")
	foreach(SPLIT ${SPLIT_STRING})
		string(SUBSTRING ${SPLIT} 0 1 FIRST_CHAR)
		string(TOUPPER ${FIRST_CHAR} FIRST_CHAR)
		string(SUBSTRING ${SPLIT} 1 -1 REMAINING)
		set(PASCAL_CASE "${PASCAL_CASE}${FIRST_CHAR}${REMAINING}")
	endforeach()
	set(${OUTPUT_STRING} ${PASCAL_CASE} PARENT_SCOPE)
endfunction()

foreach(TARGET_SOURCE ${TARGET_SOURCES})
	# Get the snake_case name of the demo.
	get_filename_component(TARGET_NAME_SNAKE_CASE ${TARGET_SOURCE} NAME_WE)

	# Convert the snake_case name to kebab-case.
	string(REPLACE "_" "-" TARGET_NAME_KEBAB_CASE ${TARGET_NAME_SNAKE_CASE})

	# Convert the kebab-case name to PascalCase.
	kebab_to_pascal(TARGET_NAME_PASCAL_CASE ${TARGET_NAME_KEBAB_CASE})
	
	message(STATUS "Adding demo: ${TARGET_NAME_PASCAL_CASE}")

	# Declare the executable.
	add_executable(${TARGET_NAME_PASCAL_CASE} "${TARGET_SOURCE}")

	# Set some target options.
	set_target_properties(${TARGET_NAME_PASCAL_CASE} PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		OUTPUT_NAME ${TARGET_NAME_KEBAB_CASE}
	)

	# Link OPAL.
	target_link_libraries(${TARGET_NAME_PASCAL_CASE} PRIVATE OPAL)
endforeach()
