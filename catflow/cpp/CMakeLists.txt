

# files
set(CATFLOW_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.tpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/util/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/source/util/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/server/*.h
        PARENT_SCOPE)
file(GLOB CATFLOW_SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.tpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/util/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/source/util/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/server/*.h
        )
set(CATFLOW_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include   PARENT_SCOPE)


# ########################################
# test
if (OPTION_TESTS)
    # files
    file(GLOB SRC_FILES_TEST
            ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/*.hpp
            )

    #
    # extrutable / Lib
    #
    add_executable(
            catflow-test
            ${CATFLOW_SRC_FILES}
            ${SRC_FILES_TEST})


    #
    # Include directories
    #
    target_include_directories(catflow-test
            PRIVATE

            ${PROJECT_BINARY_DIR}/source/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_BINARY_DIR}/include

            PUBLIC
            ${DEFAULT_INCLUDE_DIRECTORIES}
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            )

    #
    # Libraries
    #
    target_link_libraries(catflow-test
            ${DEPENDENCIES_SEASOCKS_LIB}
            pthread
            Boost::filesystem
            Boost::system
            GTest::main
            )

    if (${OPTION_BUILD_DEPS})
        add_dependencies(catflow-test ${DEFAULT_DEPENDENCIES})
    endif ()
endif ()


## include
add_subdirectory(test/uWebsocketsExample)