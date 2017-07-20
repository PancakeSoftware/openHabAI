# Target
set(TARGET ${PROJECT_NAME})
set(SRC)


# files
file(GLOB SRC_FILES
        ${SOURCE_DIR}Frontend.cpp
        ${SOURCE_DIR}main.cpp
        ${SOURCE_DIR}NeuralNetwork.cpp
        ${SOURCE_DIR}*.c
        ${INCLUDE_DIR}*.h)


#
# extrutable / Lib
#
add_executable(
        ${TARGET}
        ${SRC_FILES})

# c++11
setCpp11()



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
        )


#
# Libraries
#
target_link_libraries(${TARGET}
        ${DEFAULT_LIBRARIES}
        )
message(STATUS LinkLibs: "${DEFAULT_LIBRARIES}")
message(STATUS LinkLibDir: "${DEPENDENCIES_FREETYPE_DIR}")

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