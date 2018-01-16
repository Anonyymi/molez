# Force minimum CMake version
cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Create and init project
project(Molez CXX)
set(DIR_SRC "${PROJECT_SOURCE_DIR}/src")
set(DIR_DTA_NAME "data")
set(DIR_DTA "${PROJECT_SOURCE_DIR}/${DIR_DTA_NAME}")
set(DIR_BIN "${PROJECT_SOURCE_DIR}/bin/${CMAKE_PROJECT_NAME}.${CMAKE_GENERATOR}")

if (MSVC)
  set(DIR_INC "${PROJECT_SOURCE_DIR}/inc")
  set(DIR_LIB "${PROJECT_SOURCE_DIR}/lib/msvc-x86")

  # SDL2
  set(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIR} "${DIR_INC}")
  set(SDL2_LIBRARY ${SDL2_LIBRARY} "${DIR_LIB}/SDL2main.lib" "${DIR_LIB}/SDL2.lib" "${DIR_LIB}/SDL2_mixer.lib")
endif()

# Define used CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules")

# Enable C++ and C
enable_language(CXX)
enable_language(C)

# Print info
message(STATUS "Target GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "SDL2_INCLUDE_DIR: ${SDL2_INCLUDE_DIR}")
message(STATUS "SDL2_LIBRARY: ${SDL2_LIBRARY}")

# Include source files
file(GLOB_RECURSE SRC_CPP "${DIR_SRC}/*.cpp")
file(GLOB_RECURSE SRC_C "${DIR_SRC}/*.c")
file(GLOB_RECURSE SRC_HPP "${DIR_SRC}/*.hpp")
file(GLOB_RECURSE SRC_H "${DIR_SRC}/*.h")
file(GLOB_RECURSE SRC_ALL "${DIR_SRC}/*.cpp" "${DIR_SRC}/*.c" "${DIR_SRC}/*.hpp" "${DIR_SRC}/*.h")
file(GLOB_RECURSE SRC_INC "${DIR_INC}/*.h" "${DIR_INC}/*.hpp")

# Force CXX compiler/linker for all source files
set_source_files_properties(${SRC_CPP} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${SRC_C} PROPERTIES LANGUAGE CXX)

# Setup source file grouping for project files
source_group ("src" FILES ${SRC_ALL})

# Add to-be-linked exes/libs to project
link_directories(${DIR_LIB})
add_executable(Molez ${SRC_ALL} ${SRC_INC})

# Set default VS_STARTUP_PROJECT
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Molez)

# Setup CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Add to-be-linked dependencies
target_include_directories(Molez PUBLIC "${DIR_INC}")
target_include_directories (Molez PUBLIC "${SDL2_INCLUDE_DIR}")
find_package(SDL2 REQUIRED)
target_link_libraries(Molez PUBLIC
  ${SDL2_LIBRARY}
)

# Set project build config
add_custom_command(
	TARGET Molez POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${DIR_DTA}
			${CMAKE_CURRENT_BINARY_DIR}/${DIR_DTA_NAME}
)
add_custom_command(
	TARGET Molez POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${DIR_LIB}
			${CMAKE_CURRENT_BINARY_DIR}
)

# Set project install config
install(TARGETS Molez DESTINATION ${DIR_BIN})
install(DIRECTORY ${DIR_DTA} DESTINATION ${DIR_BIN})
install(DIRECTORY ${DIR_LIB}/ DESTINATION ${DIR_BIN})

# Set project package(s)
set(CPACK_PACKAGE_NAME "Molez")
set(CPACK_MONOLITHIC_INSTALL 1)
include(CPack)