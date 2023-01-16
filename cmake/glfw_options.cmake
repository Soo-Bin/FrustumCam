
function (build_glfw_project target prefix)

    set(trigger_build_dir ${CMAKE_BINARY_DIR}/force_${target})

    #mktemp dir in build tree
    file(MAKE_DIRECTORY ${trigger_build_dir} ${trigger_build_dir}/build)

    #generate false dependency project
    set(CMAKE_LIST_CONTENT "
        cmake_minimum_required(VERSION 3.0)
        project(ExternalProjectCustom)
        include(ExternalProject)
        ExternalProject_add(${target}
            SOURCE_DIR ${prefix}
            CMAKE_ARGS ${ARGN})
        add_custom_target(trigger_${target})
        add_dependencies(trigger_${target} ${target})
    ")

    file(WRITE ${trigger_build_dir}/CMakeLists.txt "${CMAKE_LIST_CONTENT}")
    
    execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ..
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )

endfunction()

build_glew_project(
    glfw
    ${PROJECT_SOURCE_DIR}/3rdparty/glfw
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/glfw
    -DGLFW_BUILD_DOCS=OFF
    -DGLFW_BUILD_TESTS=OFF
    -DGLFW_BUILD_EXAMPLES=OFF
)


set(GLFW_DIR ${PROJECT_SOURCE_DIR}/build/glfw)
find_package(glfw3 3.3.8 REQUIRED HINTS ${GLFW_DIR})

include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

get_target_property(GLFW_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)
set(GLFW_INCLUDE_DIR ${GLFW_INCLUDE_DIRS})

get_target_property(GLFW_LIBRARY_RELEASE glfw IMPORTED_LOCATION_RELEASE)

list(APPEND GLFW_LIBRARIES ${GLFW_LIBRARY_RELEASE})
list(APPEND GLFW_LIBRARY ${GLFW_LIBRARY_RELEASE})
select_library_configurations(GLFW)
set(GLFW_STATIC_LIBRARIES ${GLFW_LIBRARIES})

unset(GLFW_LIBRARY)
unset(GLFW_LIBRARY_RELEASE)
return()