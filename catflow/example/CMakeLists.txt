
# files
file(GLOB SRC_FILES
        *.cpp
        ${CATFLOW_SRC})

# extrutable / Lib
add_executable(
        catflow-example
        ${SRC_FILES})

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
        ${DEFAULT_LIBRARIES})