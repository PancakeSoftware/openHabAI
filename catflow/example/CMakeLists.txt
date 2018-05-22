
# files
file(GLOB SRC_FILES
        main.cpp
        ${CATFLOW_SRC})

# extrutable / Lib
add_executable(
        catflow-example
        ${SRC_FILES})

# c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Include directories
target_include_directories(catflow-example
        #PRIVATE

        #${PROJECT_BINARY_DIR}/source/include
        #${CMAKE_CURRENT_SOURCE_DIR}/include
        #${CMAKE_CURRENT_BINARY_DIR}/include

        PUBLIC
        ${DEFAULT_INCLUDE_DIRECTORIES}
        ${CATFLOW_INCLUDE}
        )

# Libraries
target_link_libraries(catflow-example
        ${DEPENDENCIES_SEASOCKS_LIB}
        ${DEPENDENCIES_UWEBSOCKETS_LIB}
        pthread
        Boost::filesystem
        Boost::system)

target_compile_definitions(catflow-example
        PUBLIC
        ${DEFAUT_DEFINITIONS})


if (${OPTION_BUILD_DEPS})
    add_dependencies(catflow-example
            uWebSockets_external
            ${DEPENDENCIES_SEASOCKS_LIB_EXTERNAL})
endif ()