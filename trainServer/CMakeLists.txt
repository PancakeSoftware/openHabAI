cmake_minimum_required(VERSION 3.0)

# Include cmake modules
include(cmake/buildHelper.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake")


# ------------------------------------
# -- Project infos

# Meta information about the project
setMetaProject(
        "OpenHabAI"  "OPENHAB_AI"               # name, name in headerfile,
        "ai for openhab"                        # descrition
        "Joshua Johannson"                      # author
)


# set project name and output dir
project(${META_PROJECT_NAME})


# ------------------------------------
# -- Project options
option(OPTION_TESTS "do tests." ON)
option(OPTION_BUILD_DEPS  "Build dependencies." ON)


# -----------------------------
# -- set BUILD out dirs
setOutDir(
        build                               # general
        build/bin                           # bin
        build/lib                           # lib
        build/include                       # include
        build/lib                           # archiv
        build/pack                          # package / installer / app
)

# ----------------------------
# c++ options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-unused-variable")

# -----------------------------
# -- set INSTALL out dirs
set(INSTALL_BIN bin)
set(INSTALL_LIB lib)
set(INSTALL_INCLUDE include)
set(INSTALL_COMPONENTS applications libraries headers)


# -----------------------------
# -- default SOURCE dirs for targets
set(SOURCE_DIR source/)
set(TEST_DIR_GLOB ${CMAKE_CURRENT_SOURCE_DIR}/test/)
set(INCLUDE_DIR include/)



# -------------------------------------------------
# -- Dependences
add_subdirectory(dependencies)

# -- BOOST
hunter_add_package(Boost COMPONENTS filesystem)
find_package(Boost CONFIG REQUIRED system filesystem)

# -- GTEST
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)


# ------------------------
link_directories(
        ${OUTPUT_LIB}
        ${DEPENDENCIES_TENSORFLOW_DIR}
        ${DEPENDENCIES_MXNET_DIR}
)


# default headers, libs
set(DEFAULT_INCLUDE_DIRECTORIES
        ${DEPENDENCIES_TENSORFLOW_INCLUDE}
        ${DEPENDENCIES_JSON_INCLUDE}
        ${DEPENDENCIES_MXNET_INCLUDE}
        ${OUTPUT_INCLUDE}
        ${DEPENDENCIES_EXPRTK_INCLUDE}
        )

set(DEFAULT_LIBRARIES
        ${DEPENDENCIES_TENSORFLOW_LIB}
        ${DEPENDENCIES_SEASOCKS_LIB}
        ${DEPENDENCIES_MXNET_LIB}
        pthread
        Boost::filesystem
        Boost::system
        )

set(DEFAULT_DEPENDENCIES
        mxnet_external
        seasocks_external
        )


# source dir
add_subdirectory(source)




# ----------------------------------
# -- create installer
if (UNIX)
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER ${META_PROJECT_AUTHOR})
endif ()

SET(CPACK_OUTPUT_FILE_PREFIX ${OUTPUT_PACKAGE})
set(CPACK_COMPONENTS_ALL ${INSTALL_COMPONENTS})
set(CPACK_PACKAGE_NAME ${META_PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${META_PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION ${META_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${META_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${META_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${META_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${META_PROJECT_NAME})

include(CPack)
