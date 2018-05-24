# Target
set(TARGET ${PROJECT_NAME})
set(SRC)


# files
file(GLOB SRC_FILES
        ${SOURCE_DIR}*/*.cpp
        ${SOURCE_DIR}*.cpp
        ${CATFLOW_SRC}
        ${INCLUDE_DIR}*/*.h)


#
# extrutable / Lib
#
add_executable(
        ${TARGET}
        ${SRC_FILES})

# c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# no json exceptions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable ")


#
# Include directories
#
target_include_directories(${TARGET}
        PRIVATE

        ${PROJECT_BINARY_DIR}/source/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/include

        PUBLIC
        ${DEFAULT_INCLUDE_DIRECTORIES}
        ${CATFLOW_INCLUDE}
        )


#
# Libraries
#
target_link_libraries(${TARGET}
        ${DEFAULT_LIBRARIES}
        )

target_compile_definitions(${TARGET}
        PUBLIC
        ${DEFAUT_DEFINITIONS})

if (${OPTION_BUILD_DEPS})
    add_dependencies(${TARGET} ${DEFAULT_DEPENDENCIES})
endif ()



# ########################################
#
# Deployment
#

# Self
install(TARGETS ${TARGET}
        RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT runtime
        LIBRARY DESTINATION ${INSTALL_LIB} COMPONENT runtime
        ARCHIVE DESTINATION ${INSTALL_ARCH} COMPONENT develope
        )

# Header files
install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_INCLUDE}
        COMPONENT develope
        )