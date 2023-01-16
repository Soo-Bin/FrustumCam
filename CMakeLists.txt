cmake_minimum_required(VERSION 3.15)
project(frustumcam)

#------------------------------------------------------------------------------
# setting cmake / c++ / env ...
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

message(STATUS "CMake version = ${CMAKE_VERSION}")
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Find 3rdparty package
include(${PROJECT_SOURCE_DIR}/cmake/glew_options.cmake)
message(STATUS "Found GLEW include path in ${GLEW_INCLUDE_DIRS}")
message(STATUS "Found GLEW library path in ${GLEW_LIBRARIES}")
add_definitions(/FI"GL/glew.h")
add_definitions(-DGLEW_STATIC)

include(${PROJECT_SOURCE_DIR}/cmake/glfw_options.cmake)
message(STATUS "Found GLFW include path in ${GLFW_INCLUDE_DIRS}")
message(STATUS "Found GLFW library path in ${GLFW_LIBRARIES}")

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL library path in ${OPENGL_LIBRARIES}")
#------------------------------------------------------------------------------

include_directories(
    ${PROJECT_SOURCE_DIR}/3rdparty
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
)

file(GLOB SRC
    ${PROJECT_SOURCE_DIR}/*.cc
)
add_executable (${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_gl_LIBRARY}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)